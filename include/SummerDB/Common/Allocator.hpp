#ifndef SUMMERDB_COMMON_ALLOCATOR_HPP
#define SUMMERDB_COMMON_ALLOCATOR_HPP

#include <limits>
#include <memory>
#include <sstream>

#include "SummerDB/Common/Helper.hpp"

namespace SummerDB {

#define MINIMUM_ALLOCATOR_BLOCK_SIZE 4096

//! The Allocator is a custom stack-based allocator that only supports bulk
//! freeing of memory It is used by most of the components of DuckDB to allocate
//! small operational objects (i.e. non-data/index related) NOTE! Objects
//! allocated by the Allocator object will NOT have their destructor called!
//! Hence do not allocate objects holding smart pointers or objects with custom
//! destructors because they will leak! NOTE! Allocator is not thread safe
//! either
class Allocator {
 public:
  void Destroy() { chunk = nullptr; }

  template <class T, typename... Args>
  T* make(Args&&... args) {
    // get space to allocate the object
    auto ptr = allocate(sizeof(T));
    // use placement-new to allocate the object
    return new (ptr) T(std::forward<Args>(args)...);
  }

  char* allocate(size_t size) {
    if (chunk && chunk->current_position + size < chunk->maximum_size) {
      auto ptr = chunk->data.get() + chunk->current_position;
      chunk->current_position += size;
      return ptr;
    }
    // have to allocate a new block
    if (size >= MINIMUM_ALLOCATOR_BLOCK_SIZE) {
      // allocate a block just for this object
      auto new_chunk = make_unique<AllocatorRegion>(size);
      // allocate the space
      auto ptr = new_chunk->data.get();
      new_chunk->current_position = size;
      // place it behind the current front object
      new_chunk->prev = move(chunk->prev);
      chunk->prev = move(new_chunk);
      return ptr;
    } else {
      // make a block of MINIMUM_ALLOCATOR_BLOCK_SIZE
      auto new_chunk =
          make_unique<AllocatorRegion>(MINIMUM_ALLOCATOR_BLOCK_SIZE);
      // allocate the space
      auto ptr = new_chunk->data.get();
      new_chunk->current_position = size;
      // append it to the end
      new_chunk->prev = move(chunk);
      chunk = move(new_chunk);
      return ptr;
    }
  }

 private:
  struct AllocatorRegion {
    AllocatorRegion(size_t size) : current_position(0), maximum_size(size) {
      data = std::unique_ptr<char[]>(new char[maximum_size]);
    }
    std::unique_ptr<char[]> data;
    size_t current_position;
    size_t maximum_size;
    std::unique_ptr<AllocatorRegion> prev;
  };
  std::unique_ptr<AllocatorRegion> chunk;
};

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_ALLOCATOR_HPP