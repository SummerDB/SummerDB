#ifndef SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a conjunction (AND/OR)
class ConjunctionExpression : public AbstractExpression {
 public:
  ConjunctionExpression(ExpressionType type,
                        std::unique_ptr<AbstractExpression> left,
                        std::unique_ptr<AbstractExpression> right)
      : AbstractExpression(type, TypeId::BOOLEAN, std::move(left),
                           std::move(right)) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CONJUNCTION_EXPRESSION_HPP