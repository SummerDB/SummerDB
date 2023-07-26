#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_CROSS_PRODUCT_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_CROSS_PRODUCT_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalCrossProduct represents a cross product between two relations
class LogicalCrossProduct : public LogicalOperator {
 public:
  LogicalCrossProduct() : LogicalOperator(LogicalOperatorType::CROSS_PRODUCT) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_CROSS_PRODUCT_HPP