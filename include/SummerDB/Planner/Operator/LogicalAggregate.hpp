#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalAggregate represents an aggregate operation with (optional) GROUP BY
//! operator.
class LogicalAggregate : public LogicalOperator {
 public:
  LogicalAggregate(std::vector<std::unique_ptr<AbstractExpression>> select_list)
      : LogicalOperator(LogicalOperatorType::AGGREGATE_AND_GROUP_BY),
        select_list(std::move(select_list)) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The selection list of group columns and aggregates
  std::vector<std::unique_ptr<AbstractExpression>> select_list;
  //! The set of groups (optional).
  std::vector<std::unique_ptr<AbstractExpression>> groups;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_AGGREGATE_HPP