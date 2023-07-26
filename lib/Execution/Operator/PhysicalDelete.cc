#include "SummerDB/Execution/Operator/PhysicalDelete.hpp"

#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Execution/ExpressionExecutor.hpp"
#include "SummerDB/Storage/DataTable.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalDelete::GetTypes() { return {TypeId::BIGINT}; }

void PhysicalDelete::_GetChunk(ClientContext& context, DataChunk& chunk,
                               PhysicalOperatorState* state) {
  chunk.Reset();

  if (state->finished) {
    return;
  }

  int64_t deleted_count = 0;
  while (true) {
    children[0]->GetChunk(context, state->child_chunk,
                          state->child_state.get());
    if (state->child_chunk.count == 0) {
      break;
    }
    // delete data in the base table
    // the row ids are given to us as the last column of the child chunk
    table.Delete(context,
                 state->child_chunk.data[state->child_chunk.column_count - 1]);
    deleted_count += state->child_chunk.count;
  }

  chunk.data[0].count = 1;
  chunk.data[0].SetValue(0, Value::BIGINT(deleted_count));
  chunk.count = 1;

  state->finished = true;
}

std::unique_ptr<PhysicalOperatorState> PhysicalDelete::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalOperatorState>(children[0].get(),
                                                 parent_executor);
}

}  // namespace SummerDB