#ifndef SUMMERDB_PARSER_EXPRESSION_CROSS_PRODUCT_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CROSS_PRODUCT_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/TableRefExpression.hpp"

namespace SummerDB {

//! Represents a cross product
class CrossProductExpression : public TableRefExpression {
 public:
  CrossProductExpression()
      : TableRefExpression(TableReferenceType::CROSS_PRODUCT) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CROSS_PRODUCT_EXPRESSION_HPP