#ifndef SUMMERDB_COMMON_TYPES_CHUNK_COLLECTION_HPP
#define SUMMERDB_COMMON_TYPES_CHUNK_COLLECTION_HPP

#include "SummerDB/Common/Types/DataChunk.hpp"

namespace SummerDB {

//!  A ChunkCollection represents a set of DataChunks that all have the same
//!  types
/*!
    A ChunkCollection represents a set of DataChunks concatenated together in a
   list. Individual values of the collection can be iterated over using the
   iterator. It is also possible to iterate directly over the chunks for more
   direct access.
*/
class ChunkCollection : public Printable {
 public:
  ChunkCollection() : count(0) {}

  //! The total amount of elements in the collection
  size_t count;
  //! The set of data chunks in the collection
  std::vector<std::unique_ptr<DataChunk>> chunks;
  //! The types of the ChunkCollection
  std::vector<TypeId> types;

  //! The amount of columns in the ChunkCollection
  size_t column_count() { return types.size(); }

  //! Append a new DataChunk directly to this ChunkCollection
  void Append(DataChunk& new_chunk);

  //! Gets the value of the column at the specified index
  Value GetValue(size_t column, size_t index);
  //! Sets the value of the column at the specified index
  void SetValue(size_t column, size_t index, Value value);

  std::string ToString() const {
    return chunks.size() == 0 ? "ChunkCollection [ 0 ]"
                              : "ChunkCollection [ " + std::to_string(count) +
                                    " ]: \n" + chunks[0]->ToString();
  }

 private:
  //! Locates the chunk that belongs to the specific index
  size_t LocateChunk(size_t index) {
    size_t result = index / STANDARD_VECTOR_SIZE;
    assert(result < chunks.size());
    return result;
  }
};

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_TYPES_CHUNK_COLLECTION_HPP