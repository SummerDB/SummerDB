#ifndef SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a boolean comparison (e.g. =, >=, <>). Always returns a boolean
//! and has two children.
class ComparisonExpression : public AbstractExpression {
 public:
  ComparisonExpression(ExpressionType type,
                       std::unique_ptr<AbstractExpression> left,
                       std::unique_ptr<AbstractExpression> right)
      : AbstractExpression(type, TypeId::BOOLEAN, std::move(left),
                           std::move(right)) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP