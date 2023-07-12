#ifndef SUMMERDB_CATALOG_COLUMN_CATALOG_HPP
#define SUMMERDB_CATALOG_COLUMN_CATALOG_HPP

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Types/Value.hpp"

namespace SummerDB {

class TableCatalogEntry;

//! A column of a table.
class ColumnCatalogEntry : public AbstractCatalogEntry {
 public:
  ColumnCatalogEntry(std::string name, TypeId type, bool is_not_null);
  ColumnCatalogEntry(std::string name, TypeId type, bool is_not_null,
                     Value default_value);

  //! The index of the column in the table
  size_t oid;
  //! The type of the column
  TypeId type;
  //! Whether or not the column can contain NULL values
  bool is_not_null;
  //! Whether or not the column has a default value
  bool has_default;
  //! The default value of the column (if any)
  Value default_value;

  virtual std::string ToString() const { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_COLUMN_CATALOG_HPP