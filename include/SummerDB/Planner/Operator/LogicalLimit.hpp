#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_LIMIT_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_LIMIT_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalLimit represents a LIMIT clause
class LogicalLimit : public LogicalOperator {
 public:
  LogicalLimit(int64_t limit, int64_t offset)
      : LogicalOperator(LogicalOperatorType::LIMIT),
        limit(limit),
        offset(offset) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The maximum amount of elements to emit
  int64_t limit;
  //! The offset from the start to begin emitting elements
  int64_t offset;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_LIMIT_HPP