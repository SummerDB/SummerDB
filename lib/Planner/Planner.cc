#include "SummerDB/Planner/Planner.hpp"

#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Core/Database.hpp"
#include "SummerDB/Parser/Statement/HeaderList.hpp"
#include "SummerDB/Planner/Binder.hpp"
#include "SummerDB/Planner/LogicalPlanGenerator.hpp"
#include "SummerDB/Planner/Operator/LogicalExplain.hpp"

namespace SummerDB {

void Planner::CreatePlan(ClientContext& context, SQLStatement& statement) {
  // first bind the tables and columns to the catalog
  Binder binder(context);

  statement.Accept(&binder);

  // now create a logical query plan from the query
  LogicalPlanGenerator logical_planner(context, *binder.bind_context);
  statement.Accept(&logical_planner);
  // logical_planner.Print();

  this->plan = std::move(logical_planner.root);
  this->context = std::move(binder.bind_context);
}

bool Planner::CreatePlan(ClientContext& context,
                         std::unique_ptr<SQLStatement> statement) {
  this->success = false;
  try {
    switch (statement->type) {
      case StatementType::INSERT:
      case StatementType::COPY:
      case StatementType::SELECT:
      case StatementType::DELETE:
      case StatementType::UPDATE:
      case StatementType::CREATE_TABLE:
        CreatePlan(context, *statement.get());
        this->success = true;
        break;
      case StatementType::DROP: {
        auto& stmt = *reinterpret_cast<DropStatement*>(statement.get());
        // TODO: create actual plan
        context.db.catalog.DropTable(context.ActiveTransaction(), stmt.schema,
                                     stmt.table);
        this->success = true;
        break;
      }
      case StatementType::TRANSACTION: {
        auto& stmt = *reinterpret_cast<TransactionStatement*>(statement.get());
        // TODO: create actual plan
        switch (stmt.type) {
          case TransactionType::BEGIN_TRANSACTION: {
            if (context.transaction.IsAutoCommit()) {
              // start the active transaction
              // if autocommit is active, we have already called
              // BeginTransaction by setting autocommit to false we
              // prevent it from being closed after this query, hence
              // preserving the transaction context for the next query
              context.transaction.SetAutoCommit(false);
            } else {
              throw TransactionException(
                  "cannot start a transaction within a transaction");
            }
            break;
          }
          case TransactionType::COMMIT: {
            if (context.transaction.IsAutoCommit()) {
              throw TransactionException(
                  "cannot commit - no transaction is active");
            } else {
              // explicitly commit the current transaction
              context.transaction.Commit();
              context.transaction.SetAutoCommit(true);
            }
            break;
          }
          case TransactionType::ROLLBACK: {
            if (context.transaction.IsAutoCommit()) {
              throw TransactionException(
                  "cannot rollback - no transaction is active");
            } else {
              // explicitly rollback the current transaction
              context.transaction.Rollback();
              context.transaction.SetAutoCommit(true);
            }
            break;
          }
          default:
            throw NotImplementedException("Unrecognized transaction type!");
        }
        this->success = true;
        break;
      }
      case StatementType::EXPLAIN: {
        auto& stmt = *reinterpret_cast<ExplainStatement*>(statement.get());
        auto parse_tree = stmt.stmt->ToString();
        CreatePlan(context, std::move(stmt.stmt));
        auto logical_plan_unopt = plan->ToString();
        auto explain = std::make_unique<LogicalExplain>(std::move(plan));
        explain->parse_tree = parse_tree;
        explain->logical_plan_unopt = logical_plan_unopt;
        plan = std::move(explain);
        this->success = true;
        break;
      }
      default:
        this->message = StringUtil::Format(
            "Statement of type %d not implemented!", statement->type);
    }
  } catch (Exception& ex) {
    this->message = ex.GetMessage();
  } catch (...) {
    this->message = "UNHANDLED EXCEPTION TYPE THROWN IN PLANNER!";
  }
  return this->success;
}

}  // namespace SummerDB