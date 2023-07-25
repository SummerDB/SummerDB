#include "SummerDB/Planner/LogicalPlanGenerator.hpp"

#include "SummerDB/Parser/Expression/ExpressionList.hpp"
#include "SummerDB/Parser/Statement/InsertStatement.hpp"
#include "SummerDB/Planner/Operator/OperatorList.hpp"

namespace SummerDB {

void LogicalPlanGenerator::Visit(SelectStatement& statement) {
  for (auto& expr : statement.select_list) {
    expr->Accept(this);
  }

  if (statement.from_table) {
    // SELECT with FROM
    statement.from_table->Accept(this);
  } else {
    // SELECT without FROM, add empty GET
    root = std::make_unique<LogicalGet>();
  }

  if (statement.where_clause) {
    statement.where_clause->Accept(this);

    auto filter =
        std::make_unique<LogicalFilter>(std::move(statement.where_clause));
    filter->children.push_back(std::move(root));
    root = std::move(filter);
  }

  if (statement.HasAggregation()) {
    auto aggregate =
        std::make_unique<LogicalAggregate>(std::move(statement.select_list));
    if (statement.HasGroup()) {
      // have to add group by columns
      aggregate->groups = std::move(statement.groupby.groups);
    }
    aggregate->children.push_back(std::move(root));
    root = std::move(aggregate);

    if (statement.HasHaving()) {
      statement.groupby.having->Accept(this);

      auto having =
          std::make_unique<LogicalFilter>(std::move(statement.groupby.having));
      having->children.push_back(std::move(root));
      root = std::move(having);
    }
  } else {
    auto projection =
        std::make_unique<LogicalProjection>(std::move(statement.select_list));
    projection->children.push_back(std::move(root));
    root = std::move(projection);
  }

  if (statement.select_distinct) {
    auto distinct = make_unique<LogicalDistinct>();
    distinct->children.push_back(std::move(root));
    root = std::move(distinct);
  }
  if (statement.HasOrder()) {
    auto order = std::make_unique<LogicalOrder>(std::move(statement.orderby));
    order->children.push_back(std::move(root));
    root = std::move(order);
  }
  if (statement.HasLimit()) {
    auto limit = std::make_unique<LogicalLimit>(statement.limit.limit,
                                                statement.limit.offset);
    limit->children.push_back(std::move(root));
    root = std::move(limit);
  }
}

static void cast_children_to_equal_types(AbstractExpression& expr) {
  if (expr.children.size() == 2) {
    TypeId left_type = expr.children[0]->return_type;
    TypeId right_type = expr.children[1]->return_type;
    if (left_type != right_type) {
      // types don't match
      // we have to add a cast
      if (left_type < right_type) {
        // add cast on left hand side
        auto cast = std::make_unique<CastExpression>(
            right_type, std::move(expr.children[0]));
        expr.children[0] = std::move(cast);
      } else {
        // add cast on right hand side
        auto cast = std::make_unique<CastExpression>(
            left_type, std::move(expr.children[1]));
        expr.children[1] = std::move(cast);
      }
    }
  }
}

void LogicalPlanGenerator::Visit(BaseTableRefExpression& expr) {
  auto table = catalog.GetTable(expr.schema_name, expr.table_name);
  auto get_table = std::make_unique<LogicalGet>(
      table, expr.alias.empty() ? expr.table_name : expr.alias);
  if (root)
    get_table->children.push_back(std::move(root));
  root = std::move(get_table);
}

void LogicalPlanGenerator::Visit(ComparisonExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  cast_children_to_equal_types(expr);
}

void LogicalPlanGenerator::Visit(ConjunctionExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  cast_children_to_equal_types(expr);
}

void LogicalPlanGenerator::Visit(JoinExpression& expr) {
  throw NotImplementedException("Joins not implemented yet!");
}

void LogicalPlanGenerator::Visit(OperatorExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  cast_children_to_equal_types(expr);
}

void LogicalPlanGenerator::Visit(SubqueryExpression& expr) {
  throw NotImplementedException("Subquery not implemented yet!");
}

void LogicalPlanGenerator::Visit(InsertStatement& statement) {
  auto table = catalog.GetTable(statement.schema, statement.table);
  auto insert =
      std::make_unique<LogicalInsert>(table, std::move(statement.values));
  root = std::move(insert);
}

}  // namespace SummerDB