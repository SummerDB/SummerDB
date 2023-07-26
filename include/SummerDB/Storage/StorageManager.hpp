#ifndef SUMMERDB_STORAGE_STORAGE_MANAGER_HPP
#define SUMMERDB_STORAGE_STORAGE_MANAGER_HPP

#include <vector>

#include "SummerDB/Common/Helper.hpp"
#include "SummerDB/Storage/DataTable.hpp"
#include "SummerDB/Storage/WriteAheadLog.hpp"

namespace SummerDB {

class Catalog;
class SummerDatabase;
class TransactionManager;

//! StorageManager is responsible for managing the physical storage of the
//! database on disk
class StorageManager {
 public:
  StorageManager(SummerDatabase& database, std::string path);
  //! Initialize a database or load an existing database from the given path
  void Initialize();
  //! Get the WAL of the StorageManager, returns nullptr if in-memory
  WriteAheadLog* GetWriteAheadLog() {
    return wal.IsInitialized() ? &wal : nullptr;
  }

 private:
  void LoadDatabase(std::string& path);

  //! The path of the database
  std::string path;
  //! The database this storagemanager belongs to
  SummerDatabase& database;
  //! The WriteAheadLog of the storage manager
  WriteAheadLog wal;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_STORAGE_MANAGER_HPP