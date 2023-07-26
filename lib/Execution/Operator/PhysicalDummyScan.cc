#include "SummerDB/Execution/Operator/PhysicalDummyScan.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalDummyScan::GetTypes() { return {TypeId::INTEGER}; }

void PhysicalDummyScan::_GetChunk(ClientContext& context, DataChunk& chunk,
                                  PhysicalOperatorState* state) {
  chunk.Reset();
  if (state->finished) {
    return;
  }
  state->finished = true;
  if (this->chunk.column_count > 0) {
    this->chunk.Move(chunk);
  } else {
    chunk.data[0].count = 1;
    chunk.count = chunk.data[0].count;
  }
}

std::unique_ptr<PhysicalOperatorState> PhysicalDummyScan::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalOperatorState>(nullptr, parent_executor);
}

}  // namespace SummerDB