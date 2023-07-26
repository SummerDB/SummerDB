#ifndef SUMMERDB_TRANSACTION_UNDO_BUFFER_HPP
#define SUMMERDB_TRANSACTION_UNDO_BUFFER_HPP

#include <memory>
#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"

namespace SummerDB {

class WriteAheadLog;

enum class UndoFlags {
  INVALID = 0,
  EMPTY_ENTRY = 1,
  CATALOG_ENTRY = 2,
  TUPLE_ENTRY = 3,
  QUERY = 4
};

struct UndoEntry {
  UndoFlags type;
  size_t length;
  std::unique_ptr<uint8_t[]> data;
};

//! The undo buffer of a transaction is used to hold previous versions of tuples
//! that might be required in the future (because of rollbacks or previous
//! transactions accessing them)
class UndoBuffer {
 public:
  UndoBuffer() {}

  //! Reserve space for an entry of the specified type and length in the undo
  //! buffer
  uint8_t* CreateEntry(UndoFlags type, size_t len);

  //! Cleanup the undo buffer
  void Cleanup();
  //! Commit the changes made in the UndoBuffer: should be called on commit
  void Commit(WriteAheadLog* log, transaction_t commit_id);
  //! Rollback the changes made in this UndoBuffer: should be called on
  //! rollback
  void Rollback();

 private:
  // List of UndoEntries, FIXME: this can be more efficient
  std::vector<UndoEntry> entries;

  UndoBuffer(const UndoBuffer&) = delete;
};

}  // namespace SummerDB

#endif  // SUMMERDB_TRANSACTION_UNDO_BUFFER_HPP