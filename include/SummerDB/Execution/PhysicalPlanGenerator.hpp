#ifndef SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP
#define SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Planner/BindContext.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"
#include "SummerDB/Planner/LogicalOperatorVisitor.hpp"

namespace SummerDB {

class ClientContext;

//! The physical plan generator generates a physical execution plan from a
//! logical query plan
class PhysicalPlanGenerator : public LogicalOperatorVisitor {
 public:
  PhysicalPlanGenerator(ClientContext& context,
                        PhysicalPlanGenerator* parent = nullptr)
      : parent(parent), context(context) {}

  bool CreatePlan(std::unique_ptr<LogicalOperator> logical);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return message; }

  void Visit(LogicalAggregate& op);
  void Visit(LogicalCreate& op);
  void Visit(LogicalCrossProduct& op);
  void Visit(LogicalDelete& op);
  void Visit(LogicalDistinct& op);
  void Visit(LogicalFilter& op);
  void Visit(LogicalGet& op);
  void Visit(LogicalJoin& op);
  void Visit(LogicalLimit& op);
  void Visit(LogicalOrder& op);
  void Visit(LogicalProjection& op);
  void Visit(LogicalInsert& op);
  void Visit(LogicalCopy& op);
  void Visit(LogicalExplain& op);
  void Visit(LogicalUnion& op);
  void Visit(LogicalUpdate& op);

  void Visit(SubqueryExpression& expr);

  void Print() { plan->Print(); }

  std::unique_ptr<PhysicalOperator> plan;

  PhysicalPlanGenerator* parent;

  bool success;
  std::string message;

 private:
  ClientContext& context;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_PHYSICAL_PLAN_GENERATOR_HPP