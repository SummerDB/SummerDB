#ifndef SUMMERDB_PARSER_EXPRESSION_CONSTANT_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CONSTANT_EXPRESSION_HPP

#include "SummerDB/Common/Types/Value.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a constant value in the query
class ConstantExpression : public AbstractExpression {
 public:
  ConstantExpression()
      : AbstractExpression(ExpressionType::VALUE_CONSTANT), value() {}
  ConstantExpression(std::string val)
      : AbstractExpression(ExpressionType::VALUE_CONSTANT, TypeId::VARCHAR),
        value(val) {}
  ConstantExpression(int32_t val)
      : AbstractExpression(ExpressionType::VALUE_CONSTANT, TypeId::INTEGER),
        value(val) {}
  ConstantExpression(double val)
      : AbstractExpression(ExpressionType::VALUE_CONSTANT, TypeId::DECIMAL),
        value(val) {}
  ConstantExpression(const Value& val)
      : AbstractExpression(ExpressionType::VALUE_CONSTANT, val.type),
        value(val) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }

  //! The constant value referenced
  Value value;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CONSTANT_EXPRESSION_HPP