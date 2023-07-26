#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalAggregate represents an aggregate operation with (optional) GROUP BY
//! operator.
class LogicalAggregate : public LogicalOperator {
 public:
  LogicalAggregate(std::vector<std::unique_ptr<Expression>> select_list)
      : LogicalOperator(LogicalOperatorType::AGGREGATE_AND_GROUP_BY,
                        std::move(select_list)) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The set of groups (optional).
  std::vector<std::unique_ptr<Expression>> groups;

  virtual size_t ExpressionCount() override {
    return expressions.size() + groups.size();
  }

  virtual Expression* GetExpression(size_t index) override {
    if (index >= ExpressionCount()) {
      throw OutOfRangeException(
          "GetExpression(): Expression index out of range!");
    }
    if (index >= expressions.size()) {
      return groups[index - expressions.size()].get();
    }
    return expressions[index].get();
  }

  virtual void SetExpression(size_t index,
                             std::unique_ptr<Expression> expr) override {
    if (index >= ExpressionCount()) {
      throw OutOfRangeException(
          "SetExpression(): Expression index out of range!");
    }
    if (index >= expressions.size()) {
      groups[index - expressions.size()] = std::move(expr);
    } else {
      expressions[index] = std::move(expr);
    }
  }

  virtual std::string ParamsToString() const override {
    std::string result = LogicalOperator::ParamsToString();
    if (groups.size() > 0) {
      result += "[";
      for (size_t i = 0; i < groups.size(); i++) {
        auto& child = groups[i];
        result += child->ToString();
        if (i < groups.size() - 1) {
          result += ", ";
        }
      }
      result += "]";
    }

    return result;
  }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP