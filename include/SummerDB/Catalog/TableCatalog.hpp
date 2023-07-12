#ifndef SUMMERDB_CATALOG_TABLE_CATALOG_HPP
#define SUMMERDB_CATALOG_TABLE_CATALOG_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/ColumnCatalog.hpp"

namespace SummerDB {

class DataTable;
class SchemaCatalogEntry;

//! A table catalog entry
class TableCatalogEntry : public AbstractCatalogEntry {
 public:
  TableCatalogEntry(Catalog* catalog, std::string name);

  //! A reference to the underlying storage unit used for this table
  DataTable* storage;
  //! A list of columns that are part of this table
  std::vector<std::shared_ptr<ColumnCatalogEntry>> columns;
  //! A map of column name to column index
  std::unordered_map<std::string, size_t> name_map;

  //! Adds a column to this table
  void AddColumn(ColumnCatalogEntry entry);
  //! Returns whether or not a column with the given name exists
  bool ColumnExists(const std::string& name);
  //! Returns a reference to the column of the specified name. Throws an
  //! exception if the column does not exist.
  std::shared_ptr<ColumnCatalogEntry> GetColumn(const std::string& name);

  virtual std::string ToString() const { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_TABLE_CATALOG_HPP