#include "SummerDB/Planner/Planner.hpp"

#include "SummerDB/Parser/Statement/CreateStatement.hpp"
#include "SummerDB/Parser/Statement/InsertStatement.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"
#include "SummerDB/Planner/Binder.hpp"
#include "SummerDB/Planner/LogicalPlanGenerator.hpp"

namespace SummerDB {

void Planner::CreatePlan(Catalog& catalog, SQLStatement& statement) {
  // first bind the tables and columns to the catalog
  Binder binder(catalog);
  statement.Accept(&binder);

  // now create a logical query plan from the query
  LogicalPlanGenerator logical_planner(catalog);
  statement.Accept(&logical_planner);
  // logical_planner.Print();

  this->plan = std::move(logical_planner.root);
  this->context = std::move(binder.context);
}

bool Planner::CreatePlan(Catalog& catalog,
                         std::unique_ptr<SQLStatement> statement) {
  this->success = false;
  try {
    switch (statement->GetType()) {
      case StatementType::INSERT:
      case StatementType::SELECT:
        CreatePlan(catalog, *statement.get());
        this->success = true;
        break;
      case StatementType::CREATE_TABLE: {
        auto& cstmt = *reinterpret_cast<CreateStatement*>(statement.get());
        // TODO: create actual plan

        if (catalog.TableExists(cstmt.schema, cstmt.table)) {
          throw BinderException("Table %s already exists in schema %s ",
                                cstmt.table.c_str(), cstmt.schema.c_str());
        }
        catalog.CreateTable(cstmt.schema, cstmt.table, cstmt.columns);
        this->success = true;
        break;
      }
      default:
        this->message = StringUtil::Format(
            "Statement of type %d not implemented!", statement->GetType());
    }
  } catch (Exception ex) {
    this->message = ex.GetMessage();
  } catch (...) {
    this->message = "UNHANDLED EXCEPTION TYPE THROWN IN PLANNER!";
  }
  return this->success;
}

}  // namespace SummerDB