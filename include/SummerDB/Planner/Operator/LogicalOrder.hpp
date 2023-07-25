#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_ORDER_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_ORDER_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalOrder represents an ORDER BY clause, sorting the data
class LogicalOrder : public LogicalOperator {
 public:
  LogicalOrder(OrderByDescription description)
      : LogicalOperator(LogicalOperatorType::ORDER_BY),
        description(std::move(description)) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  OrderByDescription description;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_ORDER_HPP