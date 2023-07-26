#include "SummerDB/Execution/PhysicalOperator.hpp"

#include "SummerDB/Core/ClientContext.hpp"

namespace SummerDB {

std::string PhysicalOperator::ToString() const {
  std::string result = PhysicalOperatorToString(type);
  if (children.size() > 0) {
    result += "(";
    for (size_t i = 0; i < children.size(); i++) {
      auto& child = children[i];
      result += child->ToString();
      if (i < children.size() - 1) {
        result += ", ";
      }
    }
    result += ")";
  }

  return result;
}

PhysicalOperatorState::PhysicalOperatorState(
    PhysicalOperator* child, ExpressionExecutor* parent_executor)
    : finished(false), parent(parent_executor) {
  if (child) {
    child->InitializeChunk(child_chunk);
    child_state = child->GetOperatorState(parent_executor);
  }
}

void PhysicalOperator::GetChunk(ClientContext& context, DataChunk& chunk,
                                PhysicalOperatorState* state) {
  context.profiler.StartOperator(this);
  _GetChunk(context, chunk, state);
  context.profiler.EndOperator(chunk);

  chunk.Verify();
}

}  // namespace SummerDB