#include "SummerDB/Execution/PhysicalPlanGenerator.hpp"

#include "SummerDB/Execution/ColumnBindingResolver.hpp"
#include "SummerDB/Execution/Operator/HeaderList.hpp"
#include "SummerDB/Parser/Expression/HeaderList.hpp"
#include "SummerDB/Planner/Operator/HeaderList.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

bool PhysicalPlanGenerator::CreatePlan(
    std::unique_ptr<LogicalOperator> logical) {
  this->success = false;
  try {
    // first resolve column references
    ColumnBindingResolver resolver;
    logical->Accept(&resolver);
    // then create the physical plan
    logical->Accept(this);
    if (!this->plan) {
      throw Exception("Unknown error in physical plan generation");
    }
    this->success = true;
  } catch (Exception& ex) {
    this->message = ex.GetMessage();
  } catch (...) {
    this->message = "UNHANDLED EXCEPTION TYPE THROWN IN PLANNER!";
  }
  return this->success;
}

void PhysicalPlanGenerator::Visit(LogicalAggregate& op) {
  LogicalOperatorVisitor::Visit(op);

  if (op.groups.size() == 0) {
    // no groups
    if (!plan) {
      // and no FROM clause, use a dummy aggregate
      auto groupby =
          std::make_unique<PhysicalHashAggregate>(std::move(op.expressions));
      this->plan = std::move(groupby);
    } else {
      // but there is a FROM clause
      // special case: aggregate entire columns together
      auto groupby =
          std::make_unique<PhysicalHashAggregate>(std::move(op.expressions));
      groupby->children.push_back(std::move(plan));
      this->plan = std::move(groupby);
    }
  } else {
    // groups! create a GROUP BY aggregator
    if (!plan) {
      throw Exception("Cannot have GROUP BY without FROM clause!");
    }

    auto groupby = std::make_unique<PhysicalHashAggregate>(std::move(op.expressions),
                                                           std::move(op.groups));
    groupby->children.push_back(std::move(plan));
    this->plan = std::move(groupby);
  }
}

void PhysicalPlanGenerator::Visit(LogicalCrossProduct& op) {
  if (plan) {
    throw Exception("Cross product should be the first node of a plan!");
  }

  assert(op.children.size() == 2);

  op.children[0]->Accept(this);
  auto left = std::move(plan);
  op.children[1]->Accept(this);
  auto right = std::move(plan);

  plan = std::make_unique<PhysicalCrossProduct>(std::move(left), std::move(right));
}

void PhysicalPlanGenerator::Visit(LogicalDelete& op) {
  LogicalOperatorVisitor::Visit(op);

  if (!plan) {
    throw Exception("Delete node cannot be the first node of a plan!");
  }

  auto del = std::make_unique<PhysicalDelete>(*op.table->storage);
  del->children.push_back(std::move(plan));
  this->plan = std::move(del);
}

void PhysicalPlanGenerator::Visit(LogicalUpdate& op) {
  LogicalOperatorVisitor::Visit(op);

  if (!plan) {
    throw Exception("Update node cannot be the first node of a plan!");
  }

  auto update = std::make_unique<PhysicalUpdate>(*op.table->storage, op.columns,
                                                 std::move(op.expressions));
  update->children.push_back(std::move(plan));
  this->plan = std::move(update);
}

void PhysicalPlanGenerator::Visit(LogicalCreate& op) {
  LogicalOperatorVisitor::Visit(op);

  if (plan) {
    throw Exception("CREATE node must be first node of the plan!");
  }

  this->plan = std::make_unique<PhysicalCreate>(op.schema, std::move(op.info));
}

void PhysicalPlanGenerator::Visit(LogicalFilter& op) {
  LogicalOperatorVisitor::Visit(op);

  if (!plan) {
    throw Exception("Filter cannot be the first node of a plan!");
  }

  auto filter = std::make_unique<PhysicalFilter>(std::move(op.expressions));
  filter->children.push_back(std::move(plan));
  this->plan = std::move(filter);
}

void PhysicalPlanGenerator::Visit(LogicalGet& op) {
  LogicalOperatorVisitor::Visit(op);

  if (!op.table) {
    this->plan = std::make_unique<PhysicalDummyScan>();
    return;
  }

  auto scan =
      std::make_unique<PhysicalTableScan>(*op.table->storage, op.column_ids);
  if (plan) {
    throw Exception("Scan has to be the first node of a plan!");
  }
  this->plan = std::move(scan);
}

void PhysicalPlanGenerator::Visit(LogicalJoin& op) {
  if (plan) {
    throw Exception("Cross product should be the first node of a plan!");
  }

  // now visit the children
  assert(op.children.size() == 2);

  op.children[0]->Accept(this);
  auto left = std::move(plan);
  op.children[1]->Accept(this);
  auto right = std::move(plan);

  for (auto& cond : op.conditions) {
    cond.left->Accept(this);
    cond.right->Accept(this);
  }

  plan = std::make_unique<PhysicalNestedLoopJoin>(
      std::move(left), std::move(right), std::move(op.conditions), op.type);
}

void PhysicalPlanGenerator::Visit(LogicalLimit& op) {
  LogicalOperatorVisitor::Visit(op);

  auto limit = std::make_unique<PhysicalLimit>(op.limit, op.offset);
  if (!plan) {
    throw Exception("Limit cannot be the first node of a plan!");
  }
  limit->children.push_back(std::move(plan));
  this->plan = std::move(limit);
}

void PhysicalPlanGenerator::Visit(LogicalOrder& op) {
  LogicalOperatorVisitor::Visit(op);

  if (!plan) {
    throw Exception("Order cannot be the first node of a plan!");
  }

  auto order = make_unique<PhysicalOrder>(std::move(op.description));
  order->children.push_back(std::move(plan));
  this->plan = std::move(order);
}

void PhysicalPlanGenerator::Visit(LogicalProjection& op) {
  LogicalOperatorVisitor::Visit(op);

  auto projection = std::make_unique<PhysicalProjection>(std::move(op.expressions));
  if (plan) {
    projection->children.push_back(std::move(plan));
  }
  this->plan = std::move(projection);
}

void PhysicalPlanGenerator::Visit(LogicalInsert& op) {
  LogicalOperatorVisitor::Visit(op);

  auto insertion = std::make_unique<PhysicalInsert>(
      op.table, std::move(op.insert_values), op.column_index_map);
  if (plan) {
    insertion->children.push_back(std::move(plan));
  }
  this->plan = std::move(insertion);
}

void PhysicalPlanGenerator::Visit(SubqueryExpression& expr) {
  PhysicalPlanGenerator generator(context, this);
  generator.CreatePlan(std::move(expr.op));
  expr.plan = std::move(generator.plan);
}

void PhysicalPlanGenerator::Visit(LogicalCopy& op) {
  LogicalOperatorVisitor::Visit(op);

  if (plan) {
    auto copy = std::make_unique<PhysicalCopy>(
        std::move(op.file_path), std::move(op.is_from), std::move(op.delimiter),
        std::move(op.quote), std::move(op.escape));
    copy->children.push_back(std::move(plan));
    this->plan = std::move(copy);
  } else {
    auto copy = std::make_unique<PhysicalCopy>(
        op.table, std::move(op.file_path), std::move(op.is_from),
        std::move(op.delimiter), std::move(op.quote), std::move(op.escape),
        std::move(op.select_list));
    this->plan = std::move(copy);
  }
}

void PhysicalPlanGenerator::Visit(LogicalExplain& op) {
  auto logical_plan_opt = op.children[0]->ToString();
  LogicalOperatorVisitor::Visit(op);

  if (plan) {
    op.physical_plan = plan->ToString();
  }

  // Construct a dummy plan that just returns the plan strings
  auto scan = std::make_unique<PhysicalDummyScan>();

  std::vector<TypeId> types = {TypeId::VARCHAR, TypeId::VARCHAR};
  scan->chunk.Initialize(types, false);
  scan->chunk.count = 3;

  scan->chunk.data[0].count = 3;
  scan->chunk.data[0].SetStringValue(0, "logical_plan");
  scan->chunk.data[0].SetStringValue(1, "logical_opt");
  scan->chunk.data[0].SetStringValue(2, "physical_plan");

  scan->chunk.data[1].count = 3;
  scan->chunk.data[1].SetStringValue(0, op.logical_plan_unopt.c_str());
  scan->chunk.data[1].SetStringValue(1, logical_plan_opt.c_str());
  scan->chunk.data[1].SetStringValue(2, op.physical_plan.c_str());

  scan->chunk.Verify();

  std::vector<std::unique_ptr<Expression>> select_list;
  select_list.push_back(
      std::make_unique<ColumnRefExpression>(TypeId::VARCHAR, 0));
  select_list.push_back(
      std::make_unique<ColumnRefExpression>(TypeId::VARCHAR, 1));

  select_list[0]->alias = "explain_key";
  select_list[1]->alias = "explain_value";

  auto projection =
      std::make_unique<PhysicalProjection>(std::move(select_list));
  projection->children.push_back(std::move(scan));
  this->plan = std::move(projection);
}

void PhysicalPlanGenerator::Visit(LogicalUnion& op) {
  assert(op.children.size() == 2);

  op.children[0]->Accept(this);
  auto top = std::move(plan);
  op.children[1]->Accept(this);
  auto bottom = std::move(plan);

  if (top->GetTypes() != bottom->GetTypes()) {
    throw Exception("Type mismatch for UNION");
  }
  plan = std::make_unique<PhysicalUnion>(std::move(top), std::move(bottom));
}

}  // namespace SummerDB