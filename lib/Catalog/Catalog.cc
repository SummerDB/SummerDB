#include "SummerDB/Catalog/Catalog.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

Catalog::Catalog()
    : AbstractCatalogEntry(nullptr, "catalog"),
      storage_manager(std::make_unique<StorageManager>()) {}

Catalog::~Catalog() {}

void Catalog::CreateSchema(const std::string& schema_name) {
  if (SchemaExists(schema_name)) {
    throw CatalogException("Schema with name %s already exists!",
                           schema_name.c_str());
  }
  schemas[schema_name] =
      std::make_shared<SchemaCatalogEntry>(this, schema_name);
}

void Catalog::CreateTable(const std::string& schema_name,
                          const std::string& table_name,
                          const std::vector<ColumnCatalogEntry>& columns) {
  if (!SchemaExists(schema_name)) {
    throw CatalogException("Schema with name %s does not exist!",
                           schema_name.c_str());
  }
  auto schema = GetSchema(schema_name);
  schema->CreateTable(table_name, columns);
}

bool Catalog::SchemaExists(const std::string& name) {
  return schemas.find(name) != schemas.end();
}

bool Catalog::TableExists(const std::string& schema_name,
                          const std::string& table_name) {
  if (!SchemaExists(schema_name)) {
    return false;
  }
  auto schema = GetSchema(schema_name);
  return schema->TableExists(table_name);
}

std::shared_ptr<SchemaCatalogEntry> Catalog::GetSchema(
    const std::string& name) {
  if (!SchemaExists(name)) {
    throw CatalogException("Schema with name %s does not exist!", name.c_str());
  }
  return schemas[name];
}

std::shared_ptr<TableCatalogEntry> Catalog::GetTable(
    const std::string& schema_name, const std::string& table_name) {
  auto schema = GetSchema(schema_name);
  return schema->GetTable(table_name);
}

}  // namespace SummerDB