#ifndef SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP

#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! Represents a boolean comparison (e.g. =, >=, <>). Always returns a boolean
//! and has two children.
class ComparisonExpression : public Expression {
 public:
  ComparisonExpression(ExpressionType type, std::unique_ptr<Expression> left,
                       std::unique_ptr<Expression> right)
      : Expression(type, TypeId::BOOLEAN, std::move(left), std::move(right)) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_COMPARISON_EXPRESSION_HPP