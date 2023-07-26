#include "SummerDB/Catalog/TableCatalog.hpp"

#include <algorithm>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Parser/Constraint/HeaderList.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

TableCatalogEntry::TableCatalogEntry(Catalog* catalog,
                                     SchemaCatalogEntry* schema,
                                     CreateTableInformation* info)
    : AbstractCatalogEntry(CatalogType::TABLE, catalog, info->table),
      schema(schema) {
  for (auto entry : info->columns) {
    if (ColumnExists(entry.name)) {
      throw CatalogException("Column with name %s already exists!",
                             entry.name.c_str());
    }

    column_t oid = columns.size();
    name_map[entry.name] = oid;
    entry.oid = oid;
    columns.push_back(entry);
  }
  storage = std::make_unique<DataTable>(catalog->storage, *this);

  // resolve the constraints
  // we have to parse the DUMMY constraints and initialize the indices
  bool has_primary_key = false;
  for (auto& constraint : info->constraints) {
    if (constraint->type == ConstraintType::DUMMY) {
      // have to resolve columns
      auto c = (ParsedConstraint*)constraint.get();
      std::vector<TypeId> types;
      std::vector<size_t> keys;
      if (c->index != (size_t)-1) {
        // column referenced by key is given by index
        types.push_back(columns[c->index].type);
        keys.push_back(c->index);
      } else {
        // have to resolve names
        for (auto& keyname : c->columns) {
          auto entry = name_map.find(keyname);
          if (entry == name_map.end()) {
            throw ParserException("column \"%s\" named in key does not exist",
                                  keyname.c_str());
          }
          if (std::find(keys.begin(), keys.end(), entry->second) !=
              keys.end()) {
            throw ParserException(
                "column \"%s\" appears twice in "
                "primary key constraint",
                keyname.c_str());
          }
          types.push_back(columns[entry->second].type);
          keys.push_back(entry->second);
        }
      }

      bool allow_null = true;
      if (c->ctype == ConstraintType::PRIMARY_KEY) {
        if (has_primary_key) {
          throw ParserException("table \"%s\" has more than one primary key",
                                name.c_str());
        }
        has_primary_key = true;
        // PRIMARY KEY constraints do not allow NULLs
        allow_null = false;
      } else {
        assert(c->ctype == ConstraintType::UNIQUE);
      }
      // initialize the index with the parsed data
      storage->indexes.push_back(
          std::make_unique<UniqueIndex>(*storage, types, keys, allow_null));
    }
    constraints.push_back(std::move(constraint));
  }
}

bool TableCatalogEntry::ColumnExists(const std::string& name) {
  return name_map.find(name) != name_map.end();
}

ColumnDefinition& TableCatalogEntry::GetColumn(const std::string& name) {
  if (!ColumnExists(name)) {
    throw CatalogException("Column with name %s does not exist!", name.c_str());
  }
  return columns[name_map[name]];
}

Statistics& TableCatalogEntry::GetStatistics(column_t oid) {
  return storage->GetStatistics(oid);
}

std::vector<TypeId> TableCatalogEntry::GetTypes() {
  std::vector<TypeId> types;
  for (auto& it : columns) {
    types.push_back(it.type);
  }
  return types;
}

}  // namespace SummerDB