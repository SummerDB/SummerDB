#include "SummerDB/Parser/Statement/SelectStatement.hpp"

#include <cassert>

namespace SummerDB {

std::string SelectStatement::ToString() const { return "Select"; }

bool SelectStatement::HasAggregation() {
  if (HasGroup()) {
    return true;
  }
  for (auto& expr : select_list) {
    if (expr->IsAggregate()) {
      return true;
    }
  }
  return false;
}

}  // namespace SummerDB