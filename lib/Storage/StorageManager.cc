#include "SummerDB/Storage/StorageManager.hpp"

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/FileSystem.hpp"
#include "SummerDB/Core/Database.hpp"
#include "SummerDB/Transaction/TransactionManager.hpp"

namespace SummerDB {

StorageManager::StorageManager(SummerDB& database, std::string path)
    : path(path), database(database), wal(database) {}

void StorageManager::Initialize() {
  bool in_memory = path.empty();

  // first initialize the base system catalogs
  // these are never written to the WAL
  auto transaction = database.transaction_manager.StartTransaction();
  database.catalog.CreateSchema(*transaction, DEFAULT_SCHEMA);
  database.transaction_manager.CommitTransaction(transaction);

  if (!in_memory) {
    // create or load the database from disk, if not in-memory mode
    LoadDatabase(path);
  }
}

void StorageManager::LoadDatabase(std::string& path) {
  // first check if the database exists
  auto wal_path = SumemrDB::JoinPath(path, WAL_FILE);
  if (!SumemrDB::DirectoryExists(path)) {
    // have to create the directory
    SumemrDB::CreateDirectory(path);
  } else {
    // directory already exists
    // verify that it is an existing database
    if (!SumemrDB::FileExists(wal_path)) {
      throw IOException(
          "Database directory exists, but could not find WAL file!");
    }
    // replay the WAL
    wal.Replay(wal_path);
  }
  // initialize the WAL file
  wal.Initialize(wal_path);
}

}  // namespace SummerDB