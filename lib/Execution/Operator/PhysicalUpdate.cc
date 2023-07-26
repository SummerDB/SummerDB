#include "SummerDB/Execution/Operator/PhysicalUpdate.hpp"

#include "SummerDB/Common/Types/VectorOperations.hpp"
#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Execution/ExpressionExecutor.hpp"
#include "SummerDB/Storage/DataTable.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalUpdate::GetTypes() { return {TypeId::BIGINT}; }

void PhysicalUpdate::_GetChunk(ClientContext& context, DataChunk& chunk,
                               PhysicalOperatorState* state) {
  chunk.Reset();

  if (state->finished) {
    return;
  }

  std::vector<TypeId> update_types;
  for (auto& expr : expressions) {
    update_types.push_back(expr->return_type);
  }
  DataChunk update_chunk;
  update_chunk.Initialize(update_types);

  int64_t updated_count = 0;
  while (true) {
    children[0]->GetChunk(context, state->child_chunk,
                          state->child_state.get());
    if (state->child_chunk.count == 0) {
      break;
    }
    // update data in the base table
    // the row ids are given to us as the last column of the child chunk
    auto& row_ids =
        state->child_chunk.data[state->child_chunk.column_count - 1];
    ExpressionExecutor executor(state->child_chunk, context);
    for (size_t i = 0; i < expressions.size(); i++) {
      auto& expr = expressions[i];
      if (expr->type == ExpressionType::VALUE_DEFAULT) {
        // resolve the default type
        auto& column = table.table.columns[columns[i]];
        update_chunk.data[i].count = state->child_chunk.count;
        VectorOperations::Set(update_chunk.data[i], column.default_value);
      } else {
        executor.Execute(expr.get(), update_chunk.data[i]);
      }
    }
    update_chunk.sel_vector = std::move(state->child_chunk.sel_vector);
    update_chunk.count = update_chunk.data[0].count;
    table.Update(context, row_ids, columns, update_chunk);
    updated_count += state->child_chunk.count;
  }

  chunk.data[0].count = 1;
  chunk.data[0].SetValue(0, Value::BIGINT(updated_count));
  chunk.count = 1;

  state->finished = true;

  chunk.Verify();
}

std::unique_ptr<PhysicalOperatorState> PhysicalUpdate::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalOperatorState>(children[0].get(),
                                                 parent_executor);
}

}  // namespace SummerDB