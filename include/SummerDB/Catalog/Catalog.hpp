#ifndef SUMMERDB_CATALOG_CATALOG_HPP
#define SUMMERDB_CATALOG_CATALOG_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/CatalogSet.hpp"
#include "SummerDB/Catalog/SchemaCatalog.hpp"
#include "SummerDB/Catalog/TableCatalog.hpp"

namespace SummerDB {

class StorageManager;

//! The Catalog object represents the catalog of the database.
class Catalog {
 public:
  Catalog(StorageManager& storage);

  //! Creates a schema in the catalog. Throws an exception if it already
  //! exists.
  void CreateSchema(Transaction& transaction,
                    const std::string& schema = DEFAULT_SCHEMA);
  //! Creates a table in the specified schema with the specified set of
  //! columns. Throws an exception if it already exists.
  void CreateTable(Transaction& transaction, CreateTableInformation* info);
  void DropTable(Transaction& transaction, const std::string& schema,
                 const std::string& table);

  //! Returns true if the schema exists, and false otherwise.
  bool SchemaExists(Transaction& transaction,
                    const std::string& name = DEFAULT_SCHEMA);
  //! Returns true if the table exists in the given schema, and false
  //! otherwise.
  bool TableExists(Transaction& transaction, const std::string& schema,
                   const std::string& table);

  //! Returns a pointer to the schema of the specified name. Throws an
  //! exception if it does not exist.
  SchemaCatalogEntry* GetSchema(Transaction& transaction,
                                const std::string& name = DEFAULT_SCHEMA);
  //! Returns a pointer to the table in the specified schema. Throws an
  //! exception if the schema or the table does not exist.
  TableCatalogEntry* GetTable(Transaction& transaction,
                              const std::string& schema,
                              const std::string& table);

  //! Reference to the storage manager
  StorageManager& storage;

 private:
  //! The catalog set holding the schemas
  CatalogSet schemas;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_CATALOG_HPP