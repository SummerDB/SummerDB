#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_DISTINCT_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_DISTINCT_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalDistinct represents a DISTINCT clause that filters duplicates from
//! the result.
class LogicalDistinct : public LogicalOperator {
 public:
  LogicalDistinct() : LogicalOperator(LogicalOperatorType::DISTINCT) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_DISTINCT_HPP