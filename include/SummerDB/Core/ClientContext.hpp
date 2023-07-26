#ifndef SUMMERDB_CORE_CLIENT_CONTEXT_HPP
#define SUMMERDB_CORE_CLIENT_CONTEXT_HPP

#include "SummerDB/Core/QueryProfiler.hpp"
#include "SummerDB/Transaction/TransactionContext.hpp"

namespace SummerDB {

class SummerDB;

//! The ClientContext holds information relevant to the current client session
//! during execution
class ClientContext {
 public:
  ClientContext(SummerDB& database);

  Transaction& ActiveTransaction() { return transaction.ActiveTransaction(); }

  //! Query profiler
  QueryProfiler profiler;
  //! The database that this client is connected to
  SummerDB& db;
  //! Data for the currently running transaction
  TransactionContext transaction;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CORE_CLIENT_CONTEXT_HPP