#ifndef SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP

#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! Represents a conjunction (AND/OR)
class ConjunctionExpression : public Expression {
 public:
  ConjunctionExpression(ExpressionType type, std::unique_ptr<Expression> left,
                        std::unique_ptr<Expression> right)
      : Expression(type, TypeId::BOOLEAN, std::move(left), std::move(right)) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP