#include "SummerDB/Execution/Operator/PhysicalTableScan.hpp"

#include "SummerDB/Core/ClientContext.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalTableScan::GetTypes() {
  return table.GetTypes(column_ids);
}

void PhysicalTableScan::_GetChunk(ClientContext& context, DataChunk& chunk,
                                  PhysicalOperatorState* state_) {
  auto state = reinterpret_cast<PhysicalTableScanOperatorState*>(state_);
  chunk.Reset();

  if (column_ids.size() == 0)
    return;

  table.Scan(context.ActiveTransaction(), chunk, column_ids,
             state->scan_offset);
}

std::unique_ptr<PhysicalOperatorState> PhysicalTableScan::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalTableScanOperatorState>(table,
                                                          parent_executor);
}

}  // namespace SummerDB