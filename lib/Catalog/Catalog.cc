#include "SummerDB/Catalog/Catalog.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

Catalog::Catalog(StorageManager& storage) : storage(storage) {}

void Catalog::CreateSchema(Transaction& transaction,
                           const std::string& schema_name) {
  auto entry = make_unique_base<AbstractCatalogEntry, SchemaCatalogEntry>(
      this, schema_name);
  if (!schemas.CreateEntry(transaction, schema_name, std::move(entry))) {
    throw CatalogException("Schema with name %s already exists!",
                           schema_name.c_str());
  }
}

bool Catalog::SchemaExists(Transaction& transaction, const std::string& name) {
  return schemas.EntryExists(transaction, name);
}

SchemaCatalogEntry* Catalog::GetSchema(Transaction& transaction,
                                       const std::string& schema_name) {
  auto entry = schemas.GetEntry(transaction, schema_name);
  if (!entry) {
    throw CatalogException("Schema with name %s does not exist!",
                           schema_name.c_str());
  }
  return (SchemaCatalogEntry*)entry;
}

bool Catalog::TableExists(Transaction& transaction,
                          const std::string& schema_name,
                          const std::string& table_name) {
  auto entry = schemas.GetEntry(transaction, schema_name);
  if (!entry) {
    return false;
  }
  SchemaCatalogEntry* schema = (SchemaCatalogEntry*)entry;
  return schema->TableExists(transaction, table_name);
}

void Catalog::CreateTable(Transaction& transaction,
                          CreateTableInformation* info) {
  auto schema = GetSchema(transaction, info->schema);
  schema->CreateTable(transaction, info);
}

void Catalog::DropTable(Transaction& transaction,
                        const std::string& schema_name,
                        const std::string& table_name) {
  auto schema = GetSchema(transaction, schema_name);
  schema->DropTable(transaction, table_name);
}

TableCatalogEntry* Catalog::GetTable(Transaction& transaction,
                                     const std::string& schema_name,
                                     const std::string& table_name) {
  auto schema = GetSchema(transaction, schema_name);
  return schema->GetTable(transaction, table_name);
}

}  // namespace SummerDB