#ifndef SUMMERDB_PARSER_EXPRESSION_DEFAULT_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_DEFAULT_EXPRESSION_HPP

#include "SummerDB/Common/Types/Value.hpp"
#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! Represents the default value of a column
class DefaultExpression : public Expression {
 public:
  DefaultExpression() : Expression(ExpressionType::VALUE_DEFAULT) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  virtual std::string ToString() const override { return "Default"; }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_DEFAULT_EXPRESSION_HPP