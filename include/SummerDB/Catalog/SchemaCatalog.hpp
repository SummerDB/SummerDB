#ifndef SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP
#define SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP

#include <string>
#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/CatalogSet.hpp"
#include "SummerDB/Catalog/TableCatalog.hpp"
#include "SummerDB/Transaction/Transaction.hpp"

namespace SummerDB {

class Catalog;
class Constraint;

//! A schema in the catalog
class SchemaCatalogEntry : public AbstractCatalogEntry {
 public:
  SchemaCatalogEntry(Catalog* catalog, std::string name);

  //! Returns true if a table with the given name exists in the schema
  bool TableExists(Transaction& transaction, const std::string& table_name);
  //! Returns a pointer to a table of the given name. Throws an exception if
  //! the table does not exist.
  TableCatalogEntry* GetTable(Transaction& transaction,
                              const std::string& table);
  //! Creates a table with the given name in the schema
  void CreateTable(Transaction& transaction, CreateTableInformation* info);

  void DropTable(Transaction& transaction, const std::string& table_name);

 private:
  //! The catalog set holding the tables
  CatalogSet tables;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_SCHEMA_CATALOG_HPP