#ifndef SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP
#define SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP

#include <string>
#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/TableCatalog.hpp"

namespace SummerDB {

class Catalog;

//! A schema in the catalog
class SchemaCatalogEntry : public AbstractCatalogEntry {
  friend class Catalog;

 public:
  SchemaCatalogEntry(Catalog* catalog, std::string name);

  //! Returns true if a table with the given name exists in the schema
  bool TableExists(const std::string& table_name);
  //! Returns a reference to a table of the given name. Throws an exception if
  //! the table does not exist.
  std::shared_ptr<TableCatalogEntry> GetTable(const std::string& table);

  //! The set of tables contained in the schema
  std::unordered_map<std::string, std::shared_ptr<TableCatalogEntry>> tables;

  virtual std::string ToString() const { return std::string(); }

 private:
  //! Creates a table with the given name and the given set of columns in the
  //! schema.
  void CreateTable(const std::string& table_name,
                   const std::vector<ColumnCatalogEntry>& columns);
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP