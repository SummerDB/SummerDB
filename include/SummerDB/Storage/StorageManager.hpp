#ifndef SUMMERDB_STORAGE_STORAGE_MANAGER_HPP
#define SUMMERDB_STORAGE_STORAGE_MANAGER_HPP

#include <vector>

#include "SummerDB/Common/Helper.hpp"
#include "SummerDB/Storage/DataTable.hpp"

namespace SummerDB {

//! StorageManager is responsible for managing the physical storage of the
//! database on disk
class StorageManager {
 public:
  //! Create a new table from a catalog entry
  void CreateTable(TableCatalogEntry& table);

  //! The set of tables managed by the storage engine
  std::vector<std::unique_ptr<DataTable>> tables;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_STORAGE_MANAGER_HPP