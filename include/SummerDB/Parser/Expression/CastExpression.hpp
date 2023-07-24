#ifndef SUMMERDB_PARSER_EXPRESSION_CAST_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CAST_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a type cast from one type to another type
class CastExpression : public AbstractExpression {
 public:
  CastExpression(TypeId target, std::unique_ptr<AbstractExpression> child)
      : AbstractExpression(ExpressionType::OPERATOR_CAST, target,
                           std::move(child)) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CAST_EXPRESSION_HPP