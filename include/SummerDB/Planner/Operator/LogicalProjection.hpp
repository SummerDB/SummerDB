#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_PROJECTION_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_PROJECTION_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalProjection represents the projection list in a SELECT clause
class LogicalProjection : public LogicalOperator {
 public:
  LogicalProjection(std::vector<std::unique_ptr<Expression>> select_list)
      : LogicalOperator(LogicalOperatorType::PROJECTION,
                        std::move(select_list)) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_PROJECTION_HPP