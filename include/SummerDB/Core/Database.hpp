#ifndef SUMMERDB_CORE_DATABASE_HPP
#define SUMMERDB_CORE_DATABASE_HPP

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Storage/StorageManager.hpp"
#include "SummerDB/Transaction/TransactionManager.hpp"

namespace SummerDB {

//! The database object. This object holds the catalog and all the
//! database-specific meta information.
class SummerDB {
 public:
  SummerDB(const char* path = nullptr);
  SummerDB(const std::string& path) : SummerDB(path.c_str()) {}

  StorageManager storage;
  Catalog catalog;
  TransactionManager transaction_manager;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CORE_DATABASE_HPP