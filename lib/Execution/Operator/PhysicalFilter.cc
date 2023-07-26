#include "SummerDB/Execution/Operator/PhysicalFilter.hpp"

#include "SummerDB/Execution/ExpressionExecutor.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalFilter::GetTypes() { return children[0]->GetTypes(); }

void PhysicalFilter::_GetChunk(ClientContext& context, DataChunk& chunk,
                               PhysicalOperatorState* state_) {
  auto state = reinterpret_cast<PhysicalOperatorState*>(state_);
  chunk.Reset();

  do {
    children[0]->GetChunk(context, state->child_chunk,
                          state->child_state.get());
    if (state->child_chunk.count == 0) {
      return;
    }

    if (expressions.size() == 0) {
      throw Exception("Attempting to execute a filter without expressions");
    }

    Vector result(TypeId::BOOLEAN, true, false);
    ExpressionExecutor executor(state, context);
    executor.Execute(expressions[0].get(), result);
    // AND together the remaining filters!
    for (size_t i = 1; i < expressions.size(); i++) {
      auto& expr = expressions[i];
      executor.Merge(expr.get(), result);
    }
    // now generate the selection vector
    chunk.sel_vector = state->child_chunk.sel_vector;
    for (size_t i = 0; i < chunk.column_count; i++) {
      // create a reference to the vector of the child chunk
      chunk.data[i].Reference(state->child_chunk.data[i]);
    }

    chunk.SetSelectionVector(result);

  } while (chunk.count == 0);
}

std::unique_ptr<PhysicalOperatorState> PhysicalFilter::GetOperatorState(
    ExpressionExecutor* parent) {
  return std::make_unique<PhysicalOperatorState>(children[0].get(), parent);
}

}  // namespace SummerDB