#include "SummerDB/Execution/Operator/PhysicalCreate.hpp"

#include "SummerDB/Catalog/SchemaCatalog.hpp"
#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Execution/ExpressionExecutor.hpp"

namespace SummerDB {

std::vector<TypeId> PhysicalCreate::GetTypes() { return {TypeId::BIGINT}; }

void PhysicalCreate::_GetChunk(ClientContext& context, DataChunk& chunk,
                               PhysicalOperatorState* state) {
  chunk.Reset();

  if (state->finished) {
    return;
  }

  int64_t inserted_count = 0;
  if (children.size() > 0) {
    // children[0]->GetChunk(context, state->child_chunk,
    //                       state->child_state.get());
    // if (state->child_chunk.count == 0) {
    // 	break;
    // }
    throw NotImplementedException("CREATE TABLE from SELECT not supported yet");
  } else {
    schema->CreateTable(context.ActiveTransaction(), info.get());
  }

  chunk.data[0].count = 1;
  chunk.data[0].SetValue(0, Value::BIGINT(inserted_count));
  chunk.count = 1;

  state->finished = true;
}

std::unique_ptr<PhysicalOperatorState> PhysicalCreate::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalOperatorState>(
      children.size() == 0 ? nullptr : children[0].get(), parent_executor);
}

}  // namespace SummerDB