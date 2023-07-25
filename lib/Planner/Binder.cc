#include "SummerDB/Planner/Binder.hpp"

#include "SummerDB/Parser/Expression/ExpressionList.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"

namespace SummerDB {

void Binder::Visit(SelectStatement& statement) {
  context = std::make_unique<BindContext>();
  // first we visit the FROM statement
  // here we determine from where we can retrieve our columns (from which
  // tables/subqueries)
  if (statement.from_table) {
    statement.from_table->Accept(this);
  }
  // now we visit the rest of the statements
  // here we performing the binding of any mentioned column names
  // back to the tables/subqueries found in the FROM statement
  // (and throw an error if a mentioned column is not found)

  // first we visit the SELECT list
  // we generate a new list of expressions because a * statement expands to
  // multiple expressions
  // note that we also gather aliases from the SELECT list
  // because they might be used in the WHERE, GROUP BY or HAVING clauses
  std::vector<std::unique_ptr<AbstractExpression>> new_select_list;
  for (auto& select_element : statement.select_list) {
    if (select_element->GetExpressionType() == ExpressionType::STAR) {
      // * statement, expand to all columns from the FROM clause
      context->GenerateAllColumnExpressions(new_select_list);
      continue;
    } else {
      // regular statement, add it to the list
      new_select_list.push_back(std::move(select_element));
    }
  }

  for (size_t i = 0; i < new_select_list.size(); i++) {
    auto& select_element = new_select_list[i];
    select_element->Accept(this);
    select_element->ResolveType();
    if (select_element->return_type == TypeId::INVALID) {
      throw BinderException("Could not resolve type of projection element!");
    }

    if (!select_element->alias.empty()) {
      context->AddExpression(select_element->alias, select_element.get(), i);
    }
  }
  statement.select_list = std::move(new_select_list);

  if (statement.where_clause) {
    statement.where_clause->Accept(this);
  }

  if (statement.HasGroup()) {
    // bind group columns
    for (auto& group : statement.groupby.groups) {
      group->Accept(this);
    }

    // handle aliases in the GROUP BY columns
    for (size_t i = 0; i < statement.groupby.groups.size(); i++) {
      auto& group = statement.groupby.groups[i];
      if (group->type != ExpressionType::COLUMN_REF) {
        throw BinderException(
            "GROUP BY clause needs to be a column or alias reference.");
      }
      auto group_column = reinterpret_cast<ColumnRefExpression*>(group.get());
      if (group_column->reference) {
        // alias reference
        // move the computation here from the SELECT clause
        size_t select_index = group_column->index;
        statement.groupby.groups[i] =
            std::move(statement.select_list[select_index]);
        // and add a GROUP REF expression to the SELECT clause
        statement.select_list[select_index] =
            std::make_unique<GroupRefExpression>(
                statement.groupby.groups[i]->return_type, i);
      }
    }

    // handle GROUP BY columns in the select clause
    for (size_t i = 0; i < statement.select_list.size(); i++) {
      auto& select = statement.select_list[i];
      if (select->type == ExpressionType::GROUP_REF)
        continue;
      if (select->IsAggregate())
        continue;

      // not an aggregate or existing GROUP REF, must point to a GROUP BY
      // column
      if (select->type != ExpressionType::COLUMN_REF) {
        // FIXME: this should be different
        // Every ColumnRef should point to a group by column OR alias
        throw Exception(
            "SELECT with GROUP BY can only contain "
            "aggregates or references to group columns!");
      }
      auto select_column = reinterpret_cast<ColumnRefExpression*>(select.get());
      if (select_column->reference) {
        throw Exception(
            "SELECT with GROUP BY can only contain "
            "aggregates or references to group columns!");
      }
      bool found_matching = false;
      for (size_t j = 0; j < statement.groupby.groups.size(); j++) {
        auto& group = statement.groupby.groups[j];
        if (group->type == ExpressionType::COLUMN_REF) {
          auto group_column =
              reinterpret_cast<ColumnRefExpression*>(group.get());
          if (group_column->index == select_column->index) {
            statement.select_list[i] = make_unique<GroupRefExpression>(
                statement.select_list[i]->return_type, j);
            found_matching = true;
            break;
          }
        }
      }
      if (!found_matching) {
        throw Exception(
            "SELECT with GROUP BY can only contain "
            "aggregates or references to group columns!");
      }
    }
  }

  if (statement.groupby.having) {
    statement.groupby.having->Accept(this);
  }
  for (auto& order : statement.orderby.orders) {
    order.expression->Accept(this);
  }
}

void Binder::Visit(BaseTableRefExpression& expr) {
  auto table = catalog.GetTable(expr.schema_name, expr.table_name);
  context->AddBaseTable(expr.alias.empty() ? expr.table_name : expr.alias,
                        table);
}

void Binder::Visit(ColumnRefExpression& expr) {
  // individual column reference
  // resolve to either a base table or a subquery expression
  if (expr.table_name.empty()) {
    // no table name: find a table or subquery that contains this
    expr.table_name = context->GetMatchingTable(expr.column_name);
  }
  auto column = context->BindColumn(expr);
}

void Binder::Visit(JoinExpression& expr) {
  expr.left->Accept(this);
  expr.right->Accept(this);
  expr.condition->Accept(this);
}

void Binder::Visit(SubqueryExpression& expr) {
  context->AddSubquery(expr.alias, expr.subquery.get());
  throw NotImplementedException("Binding subqueries not implemented yet!");
}

}  // namespace SummerDB