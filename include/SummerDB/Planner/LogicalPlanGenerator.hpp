#ifndef SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP
#define SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! The logical plan generator generates a logical query plan from a parsed SQL
//! statement
class LogicalPlanGenerator : public SQLNodeVisitor {
 public:
  LogicalPlanGenerator(Catalog& catalog) : catalog(catalog) {}

  virtual void Visit(SelectStatement& statement) override;
  virtual void Visit(InsertStatement& statement) override;

  virtual void Visit(BaseTableRefExpression& expr) override;
  virtual void Visit(ComparisonExpression& expr) override;
  virtual void Visit(ConjunctionExpression& expr) override;
  virtual void Visit(JoinExpression& expr) override;
  virtual void Visit(OperatorExpression& expr) override;
  virtual void Visit(SubqueryExpression& expr) override;

  void Print() { root->Print(); }

  //! The resulting plan
  std::unique_ptr<LogicalOperator> root;

 private:
  //! A reference to the catalog
  Catalog& catalog;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_LOGICAL_PLAN_GENERATOR_HPP