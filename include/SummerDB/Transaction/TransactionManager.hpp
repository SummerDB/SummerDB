#ifndef SUMMERDB_TRANSACTION_TRANSACTION_MANAGER_HPP
#define SUMMERDB_TRANSACTION_TRANSACTION_MANAGER_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"

namespace SummerDB {

class StorageManager;
class Transaction;

//! The Transaction Manager is responsible for creating and managing
//! transactions
class TransactionManager {
 public:
  TransactionManager(StorageManager& storage);
  //! Start a new transaction
  Transaction* StartTransaction();
  //! Commit the given transaction
  void CommitTransaction(Transaction* transaction);
  //! Rollback the given transaction
  void RollbackTransaction(Transaction* transaction);

  transaction_t GetQueryNumber() { return current_query_number++; }

 private:
  //! Remove the given transaction from the list of active transactions
  void RemoveTransaction(Transaction* transaction);

  //! The current query number
  std::atomic<transaction_t> current_query_number;
  //! The current start timestamp used by transactions
  transaction_t current_start_timestamp;
  //! The current transaction ID used by transactions
  transaction_t current_transaction_id;
  //! Set of currently running transactions
  std::vector<std::unique_ptr<Transaction>> active_transactions;
  //! Set of recently committed transactions
  std::vector<std::unique_ptr<Transaction>> recently_committed_transactions;
  //! Transactions awaiting GC
  std::vector<std::unique_ptr<Transaction>> old_transactions;
  //! The lock used for transaction operations
  std::mutex transaction_lock;
  //! The storage manager
  StorageManager& storage;
};

}  // namespace SummerDB

#endif  // SUMMERDB_TRANSACTION_TRANSACTION_MANAGER_HPP