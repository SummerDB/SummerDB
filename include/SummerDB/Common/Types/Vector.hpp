#ifndef SUMMERDB_COMMON_TYPES_VECTOR_HPP
#define SUMMERDB_COMMON_TYPES_VECTOR_HPP

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Types/StringHeap.hpp"
#include "SummerDB/Common/Types/Value.hpp"

namespace SummerDB {

//!  Vector of values of a specified TypeId.
/*!
  The vector class is the smallest unit of data used by the execution engine. It
  represents a chunk of values of a single type. A vector can either (1) own its
  own data (in which case own_data is set to true), or (2) hold only a reference
  to a set of data (e.g. the base columns or another vector).

  The execution engine operates on vectors mostly through the the set of
  available VectorOperations.

  A vector also has an optional selection vector property. When this property is
  set, the selection vector must be used to access the data as the data pointer
  itself might hold irrelevant empty entries. A simple loop for this would be as
  follows:

  for(auto i = 0; i < count; i++) {
    sum += data[sel_vector[i]];
  }

  Selection vectors are used for two purposes:

  (1) Filtering data without requiring moving and copying data around

  (2) Ordering data
*/
class Vector : public Printable {
  friend class DataChunk;

 public:
  //! The type of the elements stored in the vector.
  TypeId type;
  //! The amount of elements in the vector.
  size_t count;
  //! A pointer to the data.
  char* data;
  //! A flag indicating whether or not the vector owns its data.
  bool owns_data;
  //! The selection vector of the vector.
  sel_t* sel_vector;
  //! The null mask of the vector, if the Vector has any NULL values
  std::bitset<STANDARD_VECTOR_SIZE> nullmask;

  Vector();
  //! Create a vector of size one holding the passed on value
  Vector(Value value);
  //! Create a non-owning vector that references the specified data
  Vector(TypeId type, char* dataptr);
  //! Create an owning vector that holds at most STANDARD_VECTOR_SIZE entries.
  /*!
      Create a new vector
      If create_data is true, the vector will be an owning empty vector.
      If zero_data is true, the allocated data will be zero-initialized.
  */
  Vector(TypeId type, bool create_data, bool zero_data);
  ~Vector();

  //! Destroys the vector, deleting any owned data and resetting it to an
  //! empty vector of the specified type.
  void Destroy();

  //! Returns the [index] element of the Vector as a Value.
  Value GetValue(size_t index) const;
  //! Sets the [index] element of the Vector to the specified Value
  void SetValue(size_t index, Value val);
  //! Returns whether or not the value at the specified position is NULL
  inline bool ValueIsNull(size_t index) const {
    return nullmask[sel_vector ? sel_vector[index] : index];
  }
  //! Sets the value at the specified index to NULL
  inline void SetNull(size_t index, bool null) {
    nullmask[sel_vector ? sel_vector[index] : index] = null;
  }
  //! Sets the value of the vector at the specified index to the specified
  //! string. Can only be used for VARCHAR vectors
  void SetStringValue(size_t index, const char* value);

  //! Creates the data of this vector with the specified type. Any data that
  //! is currently in the vector is destroyed.
  void Initialize(TypeId new_type, bool zero_data = false);
  //! Casts the vector to the specified type
  void Cast(TypeId new_type = TypeId::INVALID);
  //! Appends the other vector to this vector.
  void Append(Vector& other);
  //! Copies the data from this vector to another vector.
  void Copy(Vector& other,
            size_t offset = 0);  //! Moves the data from this vector to the
                                 //! other vector. Effectively,
  //! "other" will become equivalent to this vector, and this vector will be
  //! turned into an empty vector.
  void Move(Vector& other);
  //! Flattens the vector, removing any selection vector
  void Flatten();
  //! This method guarantees that the vector becomes an owning vector
  //! If the vector is already an owning vector, it does nothing
  //! Otherwise, it copies the data
  void ForceOwnership();
  //! Causes this vector to reference the data held by the other vector.
  void Reference(Vector& other);

  //! Converts this Vector to a printable string representation
  std::string ToString() const;

  Vector(const Vector&) = delete;

  //! The stringheap of the vector
  StringHeap string_heap;

 private:
  //! If the vector owns data, this is the unique_ptr holds the actual data.
  std::unique_ptr<char[]> owned_data;
};

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_TYPES_VECTOR_HPP