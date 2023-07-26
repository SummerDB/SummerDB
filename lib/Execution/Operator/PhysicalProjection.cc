#include "SummerDB/Execution/Operator/PhysicalProjection.hpp"

#include "SummerDB/Execution/ExpressionExecutor.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalProjection::GetTypes() {
  // get the chunk types from the projection list
  std::vector<TypeId> types;
  for (auto& expr : select_list) {
    types.push_back(expr->return_type);
  }
  return types;
}

void PhysicalProjection::_GetChunk(ClientContext& context, DataChunk& chunk,
                                   PhysicalOperatorState* state) {
  chunk.Reset();

  assert(children.size() == 1);
  // get the next chunk from the child
  children[0]->GetChunk(context, state->child_chunk, state->child_state.get());
  if (state->child_chunk.count == 0) {
    return;
  }

  ExpressionExecutor executor(state, context);

  for (size_t i = 0; i < select_list.size(); i++) {
    auto& expr = select_list[i];
    executor.Execute(expr.get(), chunk.data[i]);
  }
  chunk.sel_vector = state->child_chunk.sel_vector;
  chunk.count = chunk.data[0].count;

  chunk.Verify();
}

std::unique_ptr<PhysicalOperatorState> PhysicalProjection::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalOperatorState>(
      children.size() == 0 ? nullptr : children[0].get(), parent_executor);
}

}  // namespace SummerDB