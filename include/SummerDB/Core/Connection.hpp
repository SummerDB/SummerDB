#ifndef SUMMERDB_CORE_CONNECTION_HPP
#define SUMMERDB_CORE_CONNECTION_HPP

#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Core/Result.hpp"

namespace SummerDB {

class SummerDB;

//! A connection to a database. This represents a (client) connection that can
//! be used to query the database.
class SummerDBConnection {
 public:
  SummerDBConnection(SummerDB& database);
  ~SummerDBConnection();

  std::string GetProfilingInformation() { return context.profiler.ToString(); }

  void EnableProfiling() { context.profiler.Enable(); }

  void DisableProfiling() { context.profiler.Disable(); }

  static std::unique_ptr<SummerDBResult> GetQueryResult(ClientContext& context,
                                                        std::string query);

  //! Queries the database using the transaction context of this connection
  std::unique_ptr<SummerDBResult> Query(std::string query);

  SummerDB& db;
  ClientContext context;

 private:
  std::unique_ptr<SummerDBResult> GetQueryResult(std::string query);
};

}  // namespace SummerDB

#endif  // SUMMERDB_CORE_CONNECTION_HPP