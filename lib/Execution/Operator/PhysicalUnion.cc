#include "SummerDB/Execution/Operator/PhysicalUnion.hpp"

#include "SummerDB/Common/Types/VectorOperations.hpp"

namespace SummerDB {

PhysicalUnion::PhysicalUnion(std::unique_ptr<PhysicalOperator> top,
                             std::unique_ptr<PhysicalOperator> bottom)
    : PhysicalOperator(PhysicalOperatorType::UNION) {
  children.push_back(std::move(top));
  children.push_back(std::move(bottom));
}

std::vector<TypeId> PhysicalUnion::GetTypes() { return children[0]->GetTypes(); }

// first exhaust top, then exhaust bottom. state to remember which.
void PhysicalUnion::_GetChunk(ClientContext& context, DataChunk& chunk,
                              PhysicalOperatorState* state_) {
  auto state = reinterpret_cast<PhysicalUnionOperatorState*>(state_);
  chunk.Reset();
  if (!state->top_done) {
    children[0]->GetChunk(context, chunk, state->top_state.get());
    if (chunk.count == 0) {
      state->top_done = true;
    }
  }
  if (state->top_done) {
    children[1]->GetChunk(context, chunk, state->bottom_state.get());
  }
  if (chunk.count == 0) {
    state->finished = true;
  }
  chunk.Verify();
}

std::unique_ptr<PhysicalOperatorState> PhysicalUnion::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  auto state = std::make_unique<PhysicalUnionOperatorState>(parent_executor);
  state->top_state = children[0]->GetOperatorState(parent_executor);
  state->bottom_state = children[1]->GetOperatorState(parent_executor);
  return (std::move(state));
}

}  // namespace SummerDB