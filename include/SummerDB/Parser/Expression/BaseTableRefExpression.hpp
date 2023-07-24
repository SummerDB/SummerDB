#ifndef SUMMERDB_PARSER_EXPRESSION_BASE_TABLE_REF_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_BASE_TABLE_REF_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/TableRefExpression.hpp"

namespace SummerDB {

//! Represents a TableReference to a base table in the schema
class BaseTableRefExpression : public TableRefExpression {
 public:
  BaseTableRefExpression()
      : TableRefExpression(TableReferenceType::BASE_TABLE) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }

  //! Database name, not used
  std::string database_name;
  //! Schema name
  std::string schema_name;
  //! Table name
  std::string table_name;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_BASE_TABLE_REF_EXPRESSION_HPP