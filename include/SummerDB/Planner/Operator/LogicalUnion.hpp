#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_UNION_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_UNION_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalUnion : public LogicalOperator {
 public:
  LogicalUnion(std::unique_ptr<LogicalOperator> top_select,
               std::unique_ptr<LogicalOperator> bottom_select)
      : LogicalOperator(LogicalOperatorType::UNION) {
    AddChild(std::move(top_select));
    AddChild(std::move(bottom_select));
  }

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_UNION_HPP