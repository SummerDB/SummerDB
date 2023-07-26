#include "SummerDB/Transaction/TransactionContext.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Transaction/Transaction.hpp"
#include "SummerDB/Transaction/TransactionManager.hpp"

namespace SummerDB {

TransactionContext::~TransactionContext() {
  if (current_transaction) {
    try {
      Rollback();
    } catch (...) {
    }
  }
}

void TransactionContext::BeginTransaction() {
  if (current_transaction) {
    throw TransactionException("Transaction is already running!");
  }
  current_transaction = transaction_manager.StartTransaction();
}

void TransactionContext::Commit() {
  if (!current_transaction) {
    throw TransactionException(
        "No transaction is currently active - cannot commit!");
  }
  transaction_manager.CommitTransaction(current_transaction);
  current_transaction = nullptr;
}

void TransactionContext::Rollback() {
  if (!current_transaction) {
    throw TransactionException(
        "No transaction is currently active - cannot rollback!");
  }
  transaction_manager.RollbackTransaction(current_transaction);
  current_transaction = nullptr;
}

}  // namespace SummerDB