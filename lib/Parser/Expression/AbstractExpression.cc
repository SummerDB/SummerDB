#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

bool AbstractExpression::IsAggregate() {
  bool is_aggregate = false;
  for (auto& child : children) {
    is_aggregate |= child->IsAggregate();
  }
  return is_aggregate;
}

void AbstractExpression::GetAggregates(
    std::vector<AggregateExpression*>& expressions) {
  for (auto& child : children) {
    child->GetAggregates(expressions);
  }
}

}  // namespace SummerDB