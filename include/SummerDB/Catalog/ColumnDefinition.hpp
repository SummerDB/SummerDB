#ifndef SUMMERDB_CATALOG_COLUMN_DEFINITION_HPP
#define SUMMERDB_CATALOG_COLUMN_DEFINITION_HPP

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Types/Value.hpp"

namespace SummerDB {

//! A column of a table.
class ColumnDefinition {
 public:
  ColumnDefinition(std::string name, TypeId type);
  ColumnDefinition(std::string name, TypeId type, Value default_value);

  //! The name of the entry
  std::string name;
  //! The index of the column in the table
  size_t oid;
  //! The type of the column
  TypeId type;
  //! Whether or not the column has a default value
  bool has_default;
  //! The default value of the column (if any)
  Value default_value;

  virtual std::string ToString() const { return std::string(); }
  virtual ~ColumnDefinition(){};
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_COLUMN_DEFINITION_HPP