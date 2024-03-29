#ifndef SUMMERDB_PLANNER_PLANNER_HPP
#define SUMMERDB_PLANNER_PLANNER_HPP

#include <string>
#include <vector>

#include "SummerDB/Parser/SqlStatement.hpp"
#include "SummerDB/Planner/BindContext.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class ClientContext;

class ClientContext;

//! The planner creates a logical query plan from the parsed SQL statements
//! using the Binder and LogicalPlanGenerator.
class Planner {
 public:
  bool CreatePlan(ClientContext& catalog,
                  std::unique_ptr<SQLStatement> statement);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return message; }

  bool success;
  std::string message;

  std::unique_ptr<BindContext> context;
  std::unique_ptr<LogicalOperator> plan;

 private:
  void CreatePlan(ClientContext&, SQLStatement& statement);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_PLANNER_HPP