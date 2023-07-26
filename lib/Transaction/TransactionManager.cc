#include "SummerDB/Transaction/TransactionManager.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/Helper.hpp"
#include "SummerDB/Storage/StorageManager.hpp"
#include "SummerDB/Transaction/Transaction.hpp"

namespace SummerDB {

transaction_t TRANSACTION_ID_START = 4294967296ULL;  // 2^32
transaction_t MAXIMUM_QUERY_ID =
    std::numeric_limits<transaction_t>::max();  // 2^64

TransactionManager::TransactionManager(StorageManager& storage)
    : storage(storage) {
  // start timestamp starts at zero
  current_start_timestamp = 0;
  // transaction ID starts very high:
  // it should be much higher than the current start timestamp
  // if transaction_id < start_timestamp for any set of active transactions
  // uncommited data could be read by
  current_transaction_id = TRANSACTION_ID_START;
  // the current active query id
  current_query_number = 1;
}

Transaction* TransactionManager::StartTransaction() {
  // obtain the transaction lock during this function
  std::lock_guard<std::mutex> lock(transaction_lock);

  if (current_start_timestamp >= TRANSACTION_ID_START) {
    throw Exception(
        "Cannot start more transactions, ran out of "
        "transaction identifiers!");
  }

  // obtain the start time and transaction ID of this transaction
  transaction_t start_time = current_start_timestamp++;
  transaction_t transaction_id = current_transaction_id++;

  // create the actual transaction
  auto transaction = make_unique<Transaction>(start_time, transaction_id);
  auto transaction_ptr = transaction.get();

  // store it in the set of active transactions
  active_transactions.push_back(std::move(transaction));
  return transaction_ptr;
}

void TransactionManager::CommitTransaction(Transaction* transaction) {
  // obtain the transaction lock during this function
  std::lock_guard<std::mutex> lock(transaction_lock);

  // obtain a commit id for the transaction
  transaction_t commit_id = current_start_timestamp++;

  // commit the UndoBuffer of the transaction
  transaction->Commit(storage.GetWriteAheadLog(), commit_id);

  // remove the transaction id from the list of active transactions
  // potentially resulting in garbage collection
  RemoveTransaction(transaction);
}

void TransactionManager::RollbackTransaction(Transaction* transaction) {
  // obtain the transaction lock during this function
  std::lock_guard<std::mutex> lock(transaction_lock);

  // rollback the transaction
  transaction->Rollback();

  // remove the transaction id from the list of active transactions
  // potentially resulting in garbage collection
  RemoveTransaction(transaction);
}

void TransactionManager::RemoveTransaction(Transaction* transaction) {
  // remove the transaction from the list of active transactions
  size_t t_index = active_transactions.size();
  // check for the lowest and highest start time in the list of transactions
  transaction_t lowest_start_time = TRANSACTION_ID_START;
  transaction_t lowest_active_query = MAXIMUM_QUERY_ID;
  for (size_t i = 0; i < active_transactions.size(); i++) {
    if (active_transactions[i].get() == transaction) {
      t_index = i;
    } else {
      lowest_start_time =
          std::min(lowest_start_time, active_transactions[i]->start_time);
      lowest_active_query =
          std::min(lowest_active_query, active_transactions[i]->active_query);
    }
  }
  assert(t_index != active_transactions.size());
  auto current_transaction = std::move(active_transactions[t_index]);
  if (transaction->commit_id != 0) {
    // the transaction was committed, add it to the list of recently
    // committed transactions
    recently_committed_transactions.push_back(std::move(current_transaction));
  } else {
    // the transaction was aborted, but we might still need its information
    // add it to the set of transactions awaiting GC
    current_transaction->highest_active_query = current_query_number;
    old_transactions.push_back(std::move(current_transaction));
  }
  // remove the transaction from the set of currently active transactions
  active_transactions.erase(active_transactions.begin() + t_index);
  // traverse the recently_committed transactions to see if we can remove any
  size_t i = 0;
  for (; i < recently_committed_transactions.size(); i++) {
    assert(recently_committed_transactions[i]);
    if (recently_committed_transactions[i]->commit_id < lowest_start_time) {
      // changes made BEFORE this transaction are no longer relevant
      // we can cleanup the undo buffer

      // HOWEVER: any currently running QUERY can still be using
      // the version information after the cleanup!

      // if we remove the UndoBuffer immediately, we have a race
      // condition

      // we can only safely do the actual memory cleanup when all the
      // currently active queries have finished running! (actually,
      // when all the currently active scans have finished running...)
      recently_committed_transactions[i]->Cleanup();
      // store the current highest active query
      recently_committed_transactions[i]->highest_active_query =
          current_query_number;
      // move it to the list of transactions awaiting GC
      old_transactions.push_back(std::move(recently_committed_transactions[i]));
    } else {
      // recently_committed_transactions is ordered on commit_id
      // implicitly thus if the current one is bigger than
      // lowest_start_time any subsequent ones are also bigger
      break;
    }
  }
  if (i > 0) {
    // we garbage collected transactions: remove them from the list
    recently_committed_transactions.erase(
        recently_committed_transactions.begin(),
        recently_committed_transactions.begin() + i);
  }
  // check if we can free the memory of any old transactions
  i = active_transactions.size() == 0 ? old_transactions.size() : 0;
  for (; i < old_transactions.size(); i++) {
    assert(old_transactions[i]);
    assert(old_transactions[i]->highest_active_query > 0);
    if (old_transactions[i]->highest_active_query >= lowest_active_query) {
      // there is still a query running that could be using
      // this transactions' data
      break;
    }
  }
  if (i > 0) {
    // we garbage collected transactions: remove them from the list
    old_transactions.erase(old_transactions.begin(),
                           old_transactions.begin() + i);
  }
}

}  // namespace SummerDB