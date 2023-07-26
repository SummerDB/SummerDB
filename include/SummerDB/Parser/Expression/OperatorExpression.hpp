#ifndef SUMMERDB_PARSER_EXPRESSION_OPERATOR_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_OPERATOR_EXPRESSION_HPP

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! Represents a built-in operator expression
class OperatorExpression : public Expression {
 public:
  OperatorExpression(ExpressionType type, TypeId type_id = TypeId::INVALID)
      : Expression(type, type_id) {}
  OperatorExpression(ExpressionType type, TypeId type_id,
                     std::unique_ptr<Expression> left,
                     std::unique_ptr<Expression> right = nullptr)
      : Expression(type, type_id, std::move(left), std::move(right)) {}

  virtual void ResolveType() override {
    Expression::ResolveType();
    // logical operators return a bool
    if (type == ExpressionType::OPERATOR_NOT ||
        type == ExpressionType::OPERATOR_IS_NULL ||
        type == ExpressionType::OPERATOR_IS_NOT_NULL ||
        type == ExpressionType::OPERATOR_EXISTS ||
        type == ExpressionType::COMPARE_IN) {
      return_type = TypeId::BOOLEAN;
      return;
    }
    switch (type) {
      case ExpressionType::OPERATOR_ADD:
        Statistics::Add(children[0]->stats, children[1]->stats, stats);
        break;
      case ExpressionType::OPERATOR_SUBTRACT:
        Statistics::Subtract(children[0]->stats, children[1]->stats, stats);
        break;
      case ExpressionType::OPERATOR_MULTIPLY:
        Statistics::Multiply(children[0]->stats, children[1]->stats, stats);
        break;
      case ExpressionType::OPERATOR_DIVIDE:
        Statistics::Divide(children[0]->stats, children[1]->stats, stats);
        break;
      case ExpressionType::OPERATOR_MOD:
        Statistics::Modulo(children[0]->stats, children[1]->stats, stats);
        break;
      default:
        throw NotImplementedException("Unsupported operator type!");
    }
    // return the highest type of the children, unless we need to upcast to
    // avoid overflow
    return_type = std::max(children[0]->return_type, children[1]->return_type);
    if (!stats.FitsInType(return_type)) {
      return_type = stats.MinimalType();
    }
  }

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
};
}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_OPERATOR_EXPRESSION_HPP