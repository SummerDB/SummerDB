#ifndef SUMMERDB_STORAGE_STORAGE_CHUNK_HPP
#define SUMMERDB_STORAGE_STORAGE_CHUNK_HPP

#include <atomic>
#include <mutex>
#include <vector>

#include "SummerDB/Common/Types/StringHeap.hpp"

namespace SummerDB {

class ColumnDefinition;
class DataTable;
class StorageManager;

struct VersionInformation;

class StorageChunk {
 public:
  StorageChunk(DataTable& table, size_t start);

  DataTable& table;
  bool deleted[STORAGE_CHUNK_SIZE] = {0};
  VersionInformation* version_pointers[STORAGE_CHUNK_SIZE] = {nullptr};
  std::vector<char*> columns;
  size_t count;
  size_t start;

  // Cleanup the version information of a tuple
  void Cleanup(VersionInformation* info);
  // Undo the changes made by a tuple
  void Undo(VersionInformation* info);

  //! Get an exclusive lock
  void GetExclusiveLock() {
    exclusive_lock.lock();
    while (read_count != 0)
      ;
  }

  //! Release an exclusive lock on the chunk
  void ReleaseExclusiveLock() { exclusive_lock.unlock(); }

  //! Get a shared lock on the chunk
  void GetSharedLock() {
    exclusive_lock.lock();
    read_count++;
    exclusive_lock.unlock();
  }

  //! Release a shared lock on the chunk
  void ReleaseSharedLock() { read_count--; }

  std::unique_ptr<StorageChunk> next;
  StringHeap string_heap;

 private:
  std::unique_ptr<char[]> owned_data;
  std::mutex exclusive_lock;
  std::atomic<size_t> read_count;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_STORAGE_CHUNK_HPP