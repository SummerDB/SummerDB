#ifndef SUMMERDB_CATALOG_CATALOG_HPP
#define SUMMERDB_CATALOG_CATALOG_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/ColumnCatalog.hpp"
#include "SummerDB/Catalog/SchemaCatalog.hpp"
#include "SummerDB/Catalog/TableCatalog.hpp"

namespace SummerDB {

#define DEFAULT_SCHEMA ""

class StorageManager;

//! The Catalog object represents the catalog of the database.
class Catalog : public AbstractCatalogEntry {
 public:
  Catalog();
  ~Catalog();

  //! Creates a schema in the catalog. Throws an exception if it already
  //! exists.
  void CreateSchema(const std::string& schema = DEFAULT_SCHEMA);
  //! Creates a table in the specified schema with the specified set of
  //! columns. Throws an exception if it already exists.
  void CreateTable(const std::string& schema, const std::string& table,
                   const std::vector<ColumnCatalogEntry>& columns);

  //! Returns true if the schema exists, and false otherwise.
  bool SchemaExists(const std::string& name = DEFAULT_SCHEMA);
  //! Returns true if the table exists in the given schema, and false
  //! otherwise.
  bool TableExists(const std::string& schema, const std::string& table);

  //! Returns a reference to the schema of the specified name. Throws an
  //! exception if it does not exist.
  std::shared_ptr<SchemaCatalogEntry> GetSchema(
      const std::string& name = DEFAULT_SCHEMA);
  //! Returns a reference to the table in the specified schema. Throws an
  //! exception if the table does not exist.
  std::shared_ptr<TableCatalogEntry> GetTable(const std::string& schema,
                                              const std::string& table);

  //! The set of schemas present in the catalog.
  std::unordered_map<std::string, std::shared_ptr<SchemaCatalogEntry>> schemas;

  //! The underlying storage manager that manages physical storage on disk
  std::unique_ptr<StorageManager> storage_manager;

  virtual std::string ToString() const { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_CATALOG_HPP