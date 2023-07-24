#ifndef SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/TableRefExpression.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"

namespace SummerDB {

//! Represents a subquery
class SubqueryExpression : public TableRefExpression {
 public:
  SubqueryExpression() : TableRefExpression(TableReferenceType::SUBQUERY) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }

  std::unique_ptr<SelectStatement> subquery;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP