#include "SummerDB/Core/Database.hpp"

namespace SummerDB {

SummerDB::SummerDB(const char* path)
    : storage(*this, path ? std::string(path) : std::string()),
      catalog(storage),
      transaction_manager(storage) {
  // initialize the database
  storage.Initialize();
}

}  // namespace SummerDB