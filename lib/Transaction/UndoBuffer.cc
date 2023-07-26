#include "SummerDB/Transaction/UndoBuffer.hpp"

#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/CatalogSet.hpp"
#include "SummerDB/Catalog/SchemaCatalog.hpp"
#include "SummerDB/Catalog/TableCatalog.hpp"
#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Storage/DataTable.hpp"
#include "SummerDB/Storage/StorageChunk.hpp"
#include "SummerDB/Storage/WriteAheadLog.hpp"

namespace SummerDB {

uint8_t* UndoBuffer::CreateEntry(UndoFlags type, size_t len) {
  UndoEntry entry;
  entry.type = type;
  entry.length = len;
  auto dataptr = new uint8_t[len];
  entry.data = std::unique_ptr<uint8_t[]>(dataptr);
  entries.push_back(std::move(entry));
  return dataptr;
}

void UndoBuffer::Cleanup() {
  // garbage collect everything in the Undo Chunk
  // this should only happen if
  //  (1) the transaction this UndoBuffer belongs to has successfully
  //  committed
  //      (on Rollback the Rollback() function should be called, that clears
  //      the chunks)
  //  (2) there is no active transaction with start_id < commit_id of this
  //  transaction
  for (auto& entry : entries) {
    if (entry.type == UndoFlags::CATALOG_ENTRY) {
      AbstractCatalogEntry* catalog_entry =
          *((AbstractCatalogEntry**)entry.data.get());
      // destroy the backed up entry: it is no longer required
      assert(catalog_entry->parent);
      catalog_entry->parent->child = std::move(catalog_entry->child);
    } else if (entry.type == UndoFlags::TUPLE_ENTRY) {
      // undo this entry
      auto info = (VersionInformation*)entry.data.get();
      if (info->chunk) {
        // parent refers to a storage chunk
        info->chunk->Cleanup(info);
      } else {
        // parent refers to another entry in UndoBuffer
        // simply remove this entry from the list
        auto parent = info->prev.pointer;
        parent->next = info->next;
        if (parent->next) {
          parent->next->prev.pointer = parent;
        }
      }
    } else {
      assert(entry.type == UndoFlags::EMPTY_ENTRY ||
             entry.type == UndoFlags::QUERY);
    }
  }
}

static void WriteCatalogEntry(WriteAheadLog* log, AbstractCatalogEntry* entry) {
  if (!log) {
    return;
  }
  // look at the type of the parent entry
  auto parent = entry->parent;
  switch (parent->type) {
    case CatalogType::TABLE: {
      if (parent->deleted) {
        log->WriteDropTable((TableCatalogEntry*)parent->child.get());
      } else {
        log->WriteCreateTable((TableCatalogEntry*)parent);
      }
      break;
    }
    case CatalogType::SCHEMA: {
      auto schema = (SchemaCatalogEntry*)parent;
      if (parent->deleted) {
        log->WriteDropSchema(schema);
      } else {
        log->WriteCreateSchema(schema);
      }
      break;
    }
    default:
      throw NotImplementedException(
          "UndoBuffer - don't know how to write this entry to the WAL");
  }
}

static void FlushAppends(
    WriteAheadLog* log,
    std::unordered_map<DataTable*, std::unique_ptr<DataChunk>>& appends) {
  // write appends that were not flushed yet
  assert(log);
  if (appends.size() == 0) {
    return;
  }
  for (auto& entry : appends) {
    auto dtable = entry.first;
    auto chunk = entry.second.get();
    auto& schema_name = dtable->table.schema->name;
    auto& table_name = dtable->table.name;
    log->WriteInsert(schema_name, table_name, *chunk);
  }
  appends.clear();
}

static void WriteTuple(
    WriteAheadLog* log, VersionInformation* entry,
    std::unordered_map<DataTable*, std::unique_ptr<DataChunk>>& appends) {
  if (!log) {
    return;
  }
  // we only insert tuples of insertions into the WAL
  // for deletions and updates we instead write the queries
  if (entry->tuple_data) {
    return;
  }
  // get the data for the insertion
  StorageChunk* storage = nullptr;
  DataChunk* chunk = nullptr;
  if (entry->chunk) {
    // versioninfo refers to data inside StorageChunk
    // fetch the data from the base rows
    storage = entry->chunk;
  } else {
    // insertion was updated or deleted after insertion in the same
    // transaction iterate back to the chunk to find the StorageChunk
    auto prev = entry->prev.pointer;
    while (!prev->chunk) {
      assert(entry->prev.pointer);
      prev = prev->prev.pointer;
    }
    storage = prev->chunk;
  }

  DataTable* dtable = &storage->table;
  // first lookup the chunk to which we will append this entry to
  auto append_entry = appends.find(dtable);
  if (append_entry != appends.end()) {
    // entry exists, check if we need to flush it
    chunk = append_entry->second.get();
    if (chunk->count == STANDARD_VECTOR_SIZE) {
      // entry is full: flush to WAL
      auto& schema_name = dtable->table.schema->name;
      auto& table_name = dtable->table.name;
      log->WriteInsert(schema_name, table_name, *chunk);
      chunk->Reset();
    }
  } else {
    // entry does not exist: need to make a new entry
    auto types = dtable->table.GetTypes();
    auto new_chunk = make_unique<DataChunk>();
    chunk = new_chunk.get();
    chunk->Initialize(types);
    appends.insert(make_pair(dtable, std::move(new_chunk)));
  }

  if (entry->chunk) {
    auto id = entry->prev.entry;
    // append the tuple to the current chunk
    for (size_t i = 0; i < chunk->column_count; i++) {
      auto type = chunk->data[i].type;
      size_t value_size = GetTypeIdSize(type);
      void* storage_pointer = storage->columns[i] + value_size * id;
      memcpy(chunk->data[i].data + value_size * chunk->count, storage_pointer,
             value_size);
      chunk->data[i].count++;
    }
    chunk->count++;
  } else {
    assert(entry->prev.pointer->tuple_data);
    auto tuple_data = entry->prev.pointer->tuple_data;
    // append the tuple to the current chunk
    for (size_t i = 0; i < chunk->column_count; i++) {
      auto type = chunk->data[i].type;
      size_t value_size = GetTypeIdSize(type);
      memcpy(chunk->data[i].data + value_size * chunk->count, tuple_data,
             value_size);
      tuple_data += value_size;
      chunk->data[i].count++;
    }
    chunk->count++;
  }
}

void UndoBuffer::Commit(WriteAheadLog* log, transaction_t commit_id) {
  // the list of appends committed by this transaction for each table
  std::unordered_map<DataTable*, std::unique_ptr<DataChunk>> appends;
  for (auto& entry : entries) {
    if (entry.type == UndoFlags::CATALOG_ENTRY) {
      // set the commit timestamp of the catalog entry to the given id
      AbstractCatalogEntry* catalog_entry =
          *((AbstractCatalogEntry**)entry.data.get());
      assert(catalog_entry->parent);
      catalog_entry->parent->timestamp = commit_id;

      // push the catalog update to the WAL
      WriteCatalogEntry(log, catalog_entry);
    } else if (entry.type == UndoFlags::TUPLE_ENTRY) {
      // set the commit timestamp of the entry
      auto info = (VersionInformation*)entry.data.get();
      info->version_number = commit_id;

      // push the tuple update to the WAL
      WriteTuple(log, info, appends);
    } else if (entry.type == UndoFlags::QUERY) {
      std::string query = std::string((char*)entry.data.get());
      if (log) {
        // before we write a query we write any scheduled appends
        // as the queries can reference previously appended data
        FlushAppends(log, appends);
        log->WriteQuery(query);
      }
    } else {
      throw NotImplementedException(
          "UndoBuffer - don't know how to commit this type!");
    }
  }
  if (log) {
    FlushAppends(log, appends);
    // flush the WAL
    log->Flush();
  }
}

void UndoBuffer::Rollback() {
  for (size_t i = entries.size(); i > 0; i--) {
    auto& entry = entries[i - 1];
    if (entry.type == UndoFlags::CATALOG_ENTRY) {
      // undo this catalog entry
      AbstractCatalogEntry* catalog_entry =
          *((AbstractCatalogEntry**)entry.data.get());
      assert(catalog_entry->set);
      catalog_entry->set->Undo(catalog_entry);
    } else if (entry.type == UndoFlags::TUPLE_ENTRY) {
      // undo this entry
      auto info = (VersionInformation*)entry.data.get();
      if (info->chunk) {
        // parent refers to a storage chunk
        // have to move information back into chunk
        info->chunk->Undo(info);
      } else {
        // parent refers to another entry in UndoBuffer
        // simply remove this entry from the list
        auto parent = info->prev.pointer;
        parent->next = info->next;
        if (parent->next) {
          parent->next->prev.pointer = parent;
        }
      }
    } else {
      assert(entry.type == UndoFlags::EMPTY_ENTRY ||
             entry.type == UndoFlags::QUERY);
    }
    entry.type = UndoFlags::EMPTY_ENTRY;
  }
}

}  // namespace SummerDB