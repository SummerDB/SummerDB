#ifndef SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP
#define SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Planner/BindContext.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"
#include "SummerDB/Planner/LogicalVisitor.hpp"

namespace SummerDB {

//! The physical plan generator generates a physical execution plan from a
//! logical query plan
class PhysicalPlanGenerator : public LogicalOperatorVisitor {
 public:
  PhysicalPlanGenerator(Catalog& catalog) : catalog(catalog) {}

  bool CreatePlan(std::unique_ptr<LogicalOperator> logical,
                  std::unique_ptr<BindContext> context);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return message; }

  void Visit(LogicalAggregate& op);
  void Visit(LogicalDistinct& op);
  void Visit(LogicalFilter& op);
  void Visit(LogicalGet& op);
  void Visit(LogicalLimit& op);
  void Visit(LogicalOrder& op);
  void Visit(LogicalProjection& op);
  void Visit(LogicalInsert& op);

  void Print() { plan->Print(); }

  std::unique_ptr<PhysicalOperator> plan;
  std::unique_ptr<BindContext> context;
  bool success;
  std::string message;

 private:
  Catalog& catalog;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP