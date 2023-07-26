#ifndef SUMMERDB_CORE_RESULT_HPP
#define SUMMERDB_CORE_RESULT_HPP

#include "SummerDB/Common/Types/ChunkCollection.hpp"

namespace SummerDB {

//! The result object holds the result of a query. It can either hold an error
//! message or a DataChunk that represents the return value of the column.
class SummerDBResult {
 public:
  SummerDBResult();
  SummerDBResult(std::string error);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return error; }

  void Print();

  size_t column_count() { return collection.types.size(); }
  size_t size() { return collection.count; }
  std::vector<TypeId>& types() { return collection.types; }

  ChunkCollection collection;

  bool success;
  std::string error;

 private:
  SummerDBResult(const SummerDBResult&) = delete;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CORE_RESULT_HPP