#ifndef SUMMERDB_SUMMERDB_HPP
#define SUMMERDB_SUMMERDB_HPP

#include <string>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/Types/DataChunk.hpp"

namespace SummerDB {

class SummerDB;
class SummerDBConnection;
class SummerDBResult;

//! The database object. This object holds the catalog and all the
//! database-specific meta information.
class SummerDB {
 public:
  SummerDB(const char* path);

  Catalog catalog;
};

//! A connection to a database. This represents a (client) connection that can
//! be used to query the database.
class SummerDBConnection {
 public:
  SummerDBConnection(SummerDB& database);

  std::unique_ptr<SummerDBResult> Query(const char* query);

 private:
  SummerDB& database;
};

//! The result object holds the result of a query. It can either hold an error
//! message or a DataChunk that represents the return value of the column.
class SummerDBResult {
 public:
  SummerDBResult();
  SummerDBResult(std::string error);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return error; }

  void Initialize(DataChunk& chunk);

  void Print();

  size_t count;
  std::vector<TypeId> types;
  std::vector<std::unique_ptr<DataChunk>> data;

  bool success;
  std::string error;

 private:
  SummerDBResult(const SummerDBResult&) = delete;
};

}  // namespace SummerDB

#endif  // SUMMERDB_SUMMERDB_HPP