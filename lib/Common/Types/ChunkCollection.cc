#include "SummerDB/Common/Types/ChunkCollection.hpp"

namespace SummerDB {

void ChunkCollection::Append(DataChunk& new_chunk) {
  if (new_chunk.count == 0) {
    return;
  }
  // we have to ensure that every chunk in the ChunkCollection is completely
  // filled, otherwise our O(1) lookup in GetValue and SetValue does not work
  // first fill the latest chunk, if it exists
  count += new_chunk.count;

  size_t remaining_data = new_chunk.count;
  size_t offset = 0;
  if (chunks.size() == 0) {
    // first chunk
    types = new_chunk.GetTypes();
  } else {
#ifdef DEBUG
    // the types of the new chunk should match the types of the previous one
    assert(types.size() == new_chunk.column_count);
    auto new_types = new_chunk.GetTypes();
    for (size_t i = 0; i < types.size(); i++) {
      assert(new_types[i] == types[i]);
    }
#endif

    // first append data to the current chunk
    DataChunk& last_chunk = *chunks.back();
    size_t added_data =
        std::min(remaining_data, STANDARD_VECTOR_SIZE - last_chunk.count);
    if (added_data > 0) {
      // copy <added_data> elements to the last chunk
      size_t old_count = new_chunk.count;
      new_chunk.count = added_data;
      for (size_t c = 0; c < new_chunk.column_count; c++) {
        new_chunk.data[c].count = added_data;
      }
      last_chunk.Append(new_chunk);
      remaining_data -= added_data;
      // reset the chunk to the old data
      new_chunk.count = old_count;
      for (size_t c = 0; c < new_chunk.column_count; c++) {
        new_chunk.data[c].count = old_count;
      }
      offset = added_data;
    }
  }

  if (remaining_data > 0) {
    // create a new chunk and fill it with the remainder
    auto chunk = make_unique<DataChunk>();
    chunk->Initialize(types);
    new_chunk.Copy(*chunk, offset);
    chunks.push_back(std::move(chunk));
  }
}

Value ChunkCollection::GetValue(size_t column, size_t index) {
  return chunks[LocateChunk(index)]->data[column].GetValue(
      index % STANDARD_VECTOR_SIZE);
}

void ChunkCollection::SetValue(size_t column, size_t index, Value value) {
  chunks[LocateChunk(index)]->data[column].SetValue(
      index % STANDARD_VECTOR_SIZE, value);
}

}  // namespace SummerDB