#ifndef SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP
#define SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Planner/BindContext.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class ClientContext;

//! The logical plan generator generates a logical query plan from a parsed SQL
//! statement
class LogicalPlanGenerator : public SQLNodeVisitor {
 public:
  LogicalPlanGenerator(ClientContext& context, BindContext& bind_context)
      : require_row_id(false), context(context), bind_context(bind_context) {}

  void Visit(SelectStatement& statement);
  void Visit(InsertStatement& statement);
  void Visit(CopyStatement& statement);
  void Visit(DeleteStatement& statement);
  void Visit(UpdateStatement& statement);
  void Visit(CreateTableStatement& statement);

  void Visit(AggregateExpression& expr);
  void Visit(ComparisonExpression& expr);
  void Visit(CaseExpression& expr);
  void Visit(ConjunctionExpression& expr);
  void Visit(OperatorExpression& expr);
  void Visit(SubqueryExpression& expr);

  void Visit(BaseTableRef& expr);
  void Visit(CrossProductRef& expr);
  void Visit(JoinRef& expr);
  void Visit(SubqueryRef& expr);

  void Print() { root->Print(); }

  //! The resulting plan
  std::unique_ptr<LogicalOperator> root;

 private:
  //! Whether or not we require row ids to be projected
  bool require_row_id = false;
  //! A reference to the catalog
  ClientContext& context;
  //! A reference to the current bind context
  BindContext& bind_context;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP