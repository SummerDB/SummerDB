#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_EXPLAIN_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_EXPLAIN_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalExplain : public LogicalOperator {
 public:
  LogicalExplain(std::unique_ptr<LogicalOperator> plan)
      : LogicalOperator(LogicalOperatorType::EXPLAIN) {
    children.push_back(std::move(plan));
  }

  std::string physical_plan;
  std::string parse_tree;
  std::string logical_plan_unopt;
  std::string logical_plan_opt;

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_EXPLAIN_HPP