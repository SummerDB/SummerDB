#include "SummerDB/Catalog/TableCatalog.hpp"

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

TableCatalogEntry::TableCatalogEntry(Catalog* catalog, std::string name)
    : AbstractCatalogEntry(catalog, name), storage(nullptr) {}

void TableCatalogEntry::AddColumn(ColumnCatalogEntry entry) {
  if (ColumnExists(entry.name)) {
    throw CatalogException("Column with name %s already exists!",
                           entry.name.c_str());
  }
  if (!storage) {
    throw Exception("Storage of table has not been initialized!");
  }

  size_t oid = columns.size();
  name_map[entry.name] = oid;
  entry.oid = oid;
  entry.catalog = this->catalog;
  auto column = std::make_shared<ColumnCatalogEntry>(entry);
  columns.push_back(column);
  storage->AddColumn(*column.get());
}

bool TableCatalogEntry::ColumnExists(const std::string& name) {
  return name_map.find(name) != name_map.end();
}

std::shared_ptr<ColumnCatalogEntry> TableCatalogEntry::GetColumn(
    const std::string& name) {
  if (!ColumnExists(name)) {
    throw CatalogException("Column with name %s does not exist!", name.c_str());
  }
  return columns[name_map[name]];
}

}  // namespace SummerDB