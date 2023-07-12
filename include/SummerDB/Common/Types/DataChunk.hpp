#ifndef SUMMERDB_COMMON_TYPES_DATA_CHUNK_HPP
#define SUMMERDB_COMMON_TYPES_DATA_CHUNK_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Common/Types/Vector.hpp"

namespace SummerDB {

//!  A Data Chunk represents a set of vectors.
/*!
    The data chunk class is the intermediate representation used by the
   execution engine of DuckDB. It effectively represents a subset of a relation.
   It holds a set of vectors that all have the same length.

    DataChunk is initialized using the DataChunk::Initialize function by
   providing it with a vector of TypeIds for the Vector members. By default,
   this function will also allocate a chunk of memory in the DataChunk for the
   vectors and all the vectors will be referencing vectors to the data owned by
   the chunk. The reason for this behavior is that the underlying vectors can
   become referencing vectors to other chunks as well (i.e. in the case an
   operator does not alter the data, such as a Filter operator which only adds a
   selection vector).

    In addition to holding the data of the vectors, the DataChunk also owns the
   selection vector that underlying vectors can point to.
*/
class DataChunk : public Printable {
 public:
  //! The amount of elements in the chunk. Every vector holds this amount of
  //! elements.
  size_t count;
  //! The amount of vectors that are part of this DataChunk.
  size_t column_count;
  //! The vectors owned by the DataChunk.
  std::unique_ptr<Vector[]> data;
  //! The (optional) selection vector of the DataChunk. Each of the member
  //! vectors reference this selection vector.
  sel_t* sel_vector;

  DataChunk();

#ifdef DEBUG
  //! Verify that the DataChunk is in a consistent, not corrupt state. DEBUG
  //! FUNCTION ONLY!
  void Verify();
#else
  //! Verify that the DataChunk is in a consistent, not corrupt state. DEBUG
  //! FUNCTION ONLY!
  void Verify() {}
#endif

  //! Initializes the DataChunk with the specified types to an empty DataChunk
  //! that holds at most maximum_chunk_size elements.
  //! This will create one vector of the specified type for each TypeId in the
  //! types list. The vector will be referencing vector to the data owned by
  //! the DataChunk.
  //! If zero_data is set to true, the data is zero-initialized.
  void Initialize(std::vector<TypeId>& types, bool zero_data = false);
  //! Append the other DataChunk to this one. The column count and types of
  //! the two DataChunks have to match exactly. Throws an exception if there
  //! is not enough space in the chunk.
  void Append(DataChunk& other);
  //! Destroy all data and columns owned by this DataChunk
  void Destroy();

  //! Move the data of this chunk to the other chunk
  void Move(DataChunk& other);

  //! Merges the vector new_vector with an existing selection vector (i.e.
  //! result[i] = current_vector[new_vector[i]];)
  static void MergeSelVector(sel_t* current_vector, sel_t* new_vector,
                             sel_t* result, size_t new_count);

  //! Filters elements from the vector based on a boolean vector. [True] is
  //! included, [False] and [NULL] excluded.
  void SetSelectionVector(Vector& matches);

  //! Copies the data from this vector to another vector.
  void Copy(DataChunk& other, size_t offset = 0);

  //! Removes the selection vector from the chunk
  void Flatten();

  //! Forces the Vectors of the DataChunk to own their own data
  void ForceOwnership();

  //! Resets the DataChunk to its state right after the DataChunk::Initialize
  //! function was called. This sets the count to 0, and resets each member
  //! Vector to point back to the data owned by this DataChunk.
  void Reset();

  //! Serializes a DataChunk to a stand-alone binary blob
  void Serialize(Serializer& serializer);
  //! Deserializes a blob back into a DataChunk, returns false if
  //! deserialization is not possible
  bool Deserialize(Deserializer& source);

  //! Returns a list of types of the vectors of this data chunk
  std::vector<TypeId> GetTypes();

  //! Converts this DataChunk to a printable string representation
  std::string ToString() const;

  Vector& GetVector(size_t index) { return data[index]; }

  DataChunk(const DataChunk&) = delete;

 private:
  //! The data owned by this DataChunk. This data is typically referenced by
  //! the member vectors.
  std::unique_ptr<char[]> owned_data;
  //! The selection vector of a chunk, if it owns it
  sel_t owned_sel_vector[STANDARD_VECTOR_SIZE];
};

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_TYPES_DATA_CHUNK_HPP