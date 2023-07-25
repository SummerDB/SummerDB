#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_FILTER_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_FILTER_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalFilter represents a filter operation (e.g. WHERE or HAVING clause)
class LogicalFilter : public LogicalOperator {
 public:
  LogicalFilter(std::unique_ptr<AbstractExpression> expression);

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The set of expressions that have to be satisfied for a tuple to pass the
  //! filter
  std::vector<std::unique_ptr<AbstractExpression>> expressions;

 private:
  void SplitPredicates(std::unique_ptr<AbstractExpression> expression);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_FILTER_HPP