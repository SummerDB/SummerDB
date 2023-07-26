#ifndef SUMMERDB_PARSER_PARSED_DATA_HPP
#define SUMMERDB_PARSER_PARSED_DATA_HPP

#include <vector>

#include "SummerDB/Catalog/ColumnDefinition.hpp"
#include "SummerDB/Common/InternalTypes.hpp"

namespace SummerDB {

class Constraint;

struct CreateTableInformation {
  //! Schema name to insert to
  std::string schema;
  //! Table name to insert to
  std::string table;
  //! List of columns of the table
  std::vector<ColumnDefinition> columns;
  //! List of constraints on the table
  std::vector<std::unique_ptr<Constraint>> constraints;
  //! Ignore if the entry already exists, instead of failing
  bool if_not_exists = false;

  CreateTableInformation() : schema(DEFAULT_SCHEMA), if_not_exists(false) {}
  CreateTableInformation(std::string schema, std::string table,
                         std::vector<ColumnDefinition> columns)
      : schema(schema), table(table), columns(columns), if_not_exists(false) {}
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_PARSED_DATA_HPP