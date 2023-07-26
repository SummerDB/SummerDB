#include "SummerDB/Core/Connection.hpp"

#include "SummerDB/Core/Database.hpp"
#include "SummerDB/Execution/Executor.hpp"
#include "SummerDB/Execution/PhysicalPlanGenerator.hpp"
#include "SummerDB/Optimizer/Optimizer.hpp"
#include "SummerDB/Parser/Parser.hpp"
#include "SummerDB/Planner/Planner.hpp"

namespace SummerDB {

SummerDBConnection::SummerDBConnection(SummerDatabase& database)
    : db(database), context(database) {}

SummerDBConnection::~SummerDBConnection() {}

std::unique_ptr<SummerDBResult> SummerDBConnection::GetQueryResult(
    ClientContext& context, std::string query) {
  auto result = std::make_unique<SummerDBResult>();
  result->success = false;

  context.profiler.StartQuery(query);
  try {
    // parse the query and transform it into a set of statements
    Parser parser;
    if (!parser.ParseQuery(query.c_str())) {
      return std::make_unique<SummerDBResult>(parser.GetErrorMessage());
    }

    if (parser.statements.size() > 1) {
      throw Exception("More than one statement per query not supported yet!");
    }

    auto& statement = parser.statements.back();
    if (statement->type == StatementType::UPDATE ||
        statement->type == StatementType::DELETE) {
      // log query in UNDO buffer so it can be saved in the WAL on commit
      auto& transaction = context.transaction.ActiveTransaction();
      transaction.PushQuery(query);
    }

    Planner planner;
    if (!planner.CreatePlan(context, std::move(statement))) {
      return std::make_unique<SummerDBResult>(planner.GetErrorMessage());
    }
    if (!planner.plan) {
      return std::make_unique<SummerDBResult>();
    }

    auto plan = std::move(planner.plan);

    Optimizer optimizer;
    plan = optimizer.Optimize(std::move(plan));
    if (!optimizer.GetSuccess()) {
      // failed to optimize
      return std::make_unique<SummerDBResult>(optimizer.GetErrorMessage());
    }
    if (!plan) {
      return std::make_unique<SummerDBResult>();
    }

    // now convert logical query plan into a physical query plan
    PhysicalPlanGenerator physical_planner(context);
    if (!physical_planner.CreatePlan(std::move(plan))) {
      return std::make_unique<SummerDBResult>(
          physical_planner.GetErrorMessage());
    }

    // finally execute the plan and return the result
    Executor executor;
    result->collection =
        executor.Execute(context, std::move(physical_planner.plan));
    result->success = true;
  } catch (Exception& ex) {
    result->error = ex.GetMessage();
  } catch (...) {
    result->error = "UNHANDLED EXCEPTION TYPE THROWN IN PLANNER!";
  }
  context.profiler.EndQuery();
  return result;
}

std::unique_ptr<SummerDBResult> SummerDBConnection::GetQueryResult(
    std::string query) {
  return GetQueryResult(context, query);
}

std::unique_ptr<SummerDBResult> SummerDBConnection::Query(std::string query) {
  if (context.transaction.IsAutoCommit()) {
    context.transaction.BeginTransaction();
  }

  context.ActiveTransaction().active_query =
      context.db.transaction_manager.GetQueryNumber();
  auto result = GetQueryResult(query);

  if (context.transaction.HasActiveTransaction()) {
    context.ActiveTransaction().active_query = MAXIMUM_QUERY_ID;
    try {
      if (context.transaction.IsAutoCommit()) {
        if (result->GetSuccess()) {
          context.transaction.Commit();
        } else {
          context.transaction.Rollback();
        }
      }
    } catch (Exception& ex) {
      result->success = false;
      result->error = ex.GetMessage();
    } catch (...) {
      result->success = false;
      result->error = "UNHANDLED EXCEPTION TYPE THROWN IN TRANSACTION COMMIT!";
    }
  }
  return result;
}

}  // namespace SummerDB