#include "SummerDB/Catalog/SchemaCatalog.hpp"

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

SchemaCatalogEntry::SchemaCatalogEntry(Catalog* catalog, std::string name)
    : AbstractCatalogEntry(catalog, name) {}

void SchemaCatalogEntry::CreateTable(
    const std::string& table_name,
    const std::vector<ColumnCatalogEntry>& columns) {
  if (TableExists(table_name)) {
    throw CatalogException("Table with name %s already exists!",
                           table_name.c_str());
  }
  auto table = std::make_shared<TableCatalogEntry>(catalog, table_name);
  catalog->storage_manager->CreateTable(*table.get());
  for (auto& column : columns) {
    table->AddColumn(column);
  }
  tables[table_name] = table;
}

bool SchemaCatalogEntry::TableExists(const std::string& table_name) {
  return tables.find(table_name) != tables.end();
}

std::shared_ptr<TableCatalogEntry> SchemaCatalogEntry::GetTable(
    const std::string& name) {
  if (!TableExists(name)) {
    throw CatalogException("Table with name %s does not exist!", name.c_str());
  }
  return tables[name];
}

}  // namespace SummerDB