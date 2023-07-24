#ifndef SUMMERDB_PARSER_EXPRESSION_TABLE_REF_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_TABLE_REF_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a generic expression that returns a table.
class TableRefExpression : public AbstractExpression {
 public:
  TableRefExpression(TableReferenceType ref_type)
      : AbstractExpression(ExpressionType::TABLE_REF), ref_type(ref_type) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  TableReferenceType ref_type;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_TABLE_REF_EXPRESSION_HPP