#include "SummerDB/Optimizer/Optimizer.hpp"

#include "SummerDB/Planner/Operator/HeaderList.hpp"

namespace SummerDB {

Optimizer::Optimizer() : success(false) {}

std::unique_ptr<LogicalOperator> Optimizer::Optimize(
    std::unique_ptr<LogicalOperator> plan) {
  success = false;
  try {
    success = true;
    return plan;
  } catch (Exception& ex) {
    this->message = ex.GetMessage();
  } catch (...) {
    this->message = "UNHANDLED EXCEPTION TYPE THROWN IN PLANNER!";
  }
  return nullptr;
}

}  // namespace SummerDB