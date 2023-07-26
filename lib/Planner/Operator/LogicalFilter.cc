#include "SummerDB/Planner/Operator/LogicalFilter.hpp"

namespace SummerDB {

// Split a set of predicates separate by AND statements
// These are the predicates that are safe to push down because all of them MUST
// be true
void LogicalFilter::SplitPredicates(std::unique_ptr<Expression> expression) {
  if (expression->GetExpressionType() == ExpressionType::CONJUNCTION_AND) {
    // Traverse down the expression tree along conjunction
    for (auto& child : expression->children) {
      SplitPredicates(std::move(child));
    }
  } else {
    // Find an expression that is the child of conjunction expression
    expressions.push_back(std::move(expression));
  }
}

LogicalFilter::LogicalFilter(std::unique_ptr<Expression> expression)
    : LogicalOperator(LogicalOperatorType::FILTER) {
  SplitPredicates(std::move(expression));
}

LogicalFilter::LogicalFilter() : LogicalOperator(LogicalOperatorType::FILTER) {}

}  // namespace SummerDB