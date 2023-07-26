#include "SummerDB/Execution/Executor.hpp"

namespace SummerDB {

ChunkCollection Executor::Execute(ClientContext& context,
                                  std::unique_ptr<PhysicalOperator> plan) {
  ChunkCollection result;
  // the chunk and state are used to iterate over the input plan
  auto state = plan->GetOperatorState(nullptr);

  // loop until we have retrieved all data
  std::unique_ptr<DataChunk> chunk;
  do {
    chunk = make_unique<DataChunk>();
    plan->InitializeChunk(*chunk.get());
    plan->GetChunk(context, *chunk, state.get());
    result.Append(*chunk);
  } while (chunk->count > 0);
  return result;
}

}  // namespace SummerDB