#ifndef SUMMERDB_STORAGE_DATA_TABLE_HPP
#define SUMMERDB_STORAGE_DATA_TABLE_HPP

#include <atomic>
#include <mutex>
#include <vector>

#include "SummerDB/Common/Types/DataChunk.hpp"
#include "SummerDB/Common/Types/Statistics.hpp"
#include "SummerDB/Common/Types/Tuple.hpp"
#include "SummerDB/Storage/StorageChunk.hpp"
#include "SummerDB/Storage/UniqueIndex.hpp"

namespace SummerDB {

class ClientContext;
class ColumnDefinition;
class StorageManager;
class TableCatalogEntry;
class Transaction;

struct ScanStructure {
  StorageChunk* chunk;
  size_t offset;
};

//! DataTable represents a physical table on disk
class DataTable {
  friend class UniqueIndex;

 public:
  DataTable(StorageManager& storage, TableCatalogEntry& table);

  void InitializeScan(ScanStructure& structure);
  //! Scans up to STANDARD_VECTOR_SIZE elements from the table starting
  // from offset and store them in result. Offset is incremented with how many
  // elements were returned.
  void Scan(Transaction& transaction, DataChunk& result,
            const std::vector<column_t>& column_ids, ScanStructure& structure);
  //! Append a DataChunk to the table. Throws an exception if the columns
  // don't match the tables' columns.
  void Append(ClientContext& context, DataChunk& chunk);
  //! Delete the entries with the specified row identifier from the table
  void Delete(ClientContext& context, Vector& row_ids);
  //! Update the entries with the specified row identifier from the table
  void Update(ClientContext& context, Vector& row_ids,
              std::vector<column_t>& column_ids, DataChunk& data);

  //! Get statistics of the specified column
  Statistics& GetStatistics(column_t oid) {
    if (oid == COLUMN_IDENTIFIER_ROW_ID) {
      return rowid_statistics;
    }
    return statistics[oid];
  }

  std::vector<TypeId> GetTypes(const std::vector<column_t>& column_ids);

  //! Total per-tuple size of the table
  size_t tuple_size;
  //! Accumulative per-tuple size
  std::vector<size_t> accumulative_tuple_size;
  //! A reference to the catalog table entry
  TableCatalogEntry& table;

  //! Tuple serializer for this table
  TupleSerializer serializer;
  //! A reference to the base storage manager
  StorageManager& storage;

  StorageChunk* GetChunk(size_t row_number);

  //! Unique indexes
  std::vector<std::unique_ptr<UniqueIndex>> indexes;

 private:
  //! Verify whether or not a new chunk violates any constraints
  void VerifyConstraints(ClientContext& context, DataChunk& new_chunk);
  //! The stored data of the table
  std::unique_ptr<StorageChunk> chunk_list;
  //! A reference to the last entry in the chunk list
  StorageChunk* tail_chunk;
  //! Row ID statistics
  Statistics rowid_statistics;
  //! The statistics of each of the columns
  std::unique_ptr<Statistics[]> statistics;
  //! Locks used for updating the statistics
  std::unique_ptr<std::mutex[]> statistics_locks;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_DATA_TABLE_HPP