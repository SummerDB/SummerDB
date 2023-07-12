#include "SummerDB/Parser/Expression/AggregateExpression.hpp"

namespace SummerDB {

void AggregateExpression::GetAggregates(
    std::vector<AggregateExpression*>& expressions) {
  size_t size = expressions.size();
  AbstractExpression::GetAggregates(expressions);
  if (size == expressions.size()) {
    // we only want the lowest level aggregates
    expressions.push_back(this);
  }
}

}  // namespace SummerDB