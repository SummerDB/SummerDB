#include "SummerDB/Planner/LogicalPlanGenerator.hpp"

#include <map>

#include "SummerDB/Core/ClientContext.hpp"
#include "SummerDB/Core/Database.hpp"
#include "SummerDB/Parser/Expression/HeaderList.hpp"
#include "SummerDB/Parser/Statement/HeaderList.hpp"
#include "SummerDB/Parser/Tableref/HeaderList.hpp"
#include "SummerDB/Planner/Operator/HeaderList.hpp"

namespace SummerDB {

static bool has_select_list(LogicalOperatorType type) {
  return type == LogicalOperatorType::PROJECTION ||
         type == LogicalOperatorType::AGGREGATE_AND_GROUP_BY ||
         type == LogicalOperatorType::UNION;
}

void LogicalPlanGenerator::Visit(CreateTableStatement& statement) {
  if (root) {
    throw Exception("CREATE TABLE from SELECT not supported yet!");
  }
  // bind the schema
  auto schema = context.db.catalog.GetSchema(context.ActiveTransaction(),
                                             statement.info->schema);
  // create the logical operator
  root = std::make_unique<LogicalCreate>(schema, std::move(statement.info));
}

void LogicalPlanGenerator::Visit(UpdateStatement& statement) {
  // we require row ids for the deletion
  require_row_id = true;
  // create the table scan
  statement.table->Accept(this);
  if (!root || root->type != LogicalOperatorType::GET) {
    throw Exception("Cannot create update node without table scan!");
  }
  auto get = (LogicalGet*)root.get();
  // create the filter (if any)
  if (statement.condition) {
    statement.condition->Accept(this);
    auto filter =
        std::make_unique<LogicalFilter>(std::move(statement.condition));
    filter->AddChild(std::move(root));
    root = std::move(filter);
  }
  // scan the table for the referenced columns in the update clause
  auto& table = get->table;
  std::vector<column_t> column_ids;
  for (size_t i = 0; i < statement.columns.size(); i++) {
    auto& colname = statement.columns[i];

    if (!table->ColumnExists(colname)) {
      throw BinderException("Referenced update column %s not found in table!",
                            colname.c_str());
    }
    auto& column = table->GetColumn(colname);
    column_ids.push_back(column.oid);
    if (statement.expressions[i]->type == ExpressionType::VALUE_DEFAULT) {
      // resolve the type of the DEFAULT expression
      statement.expressions[i]->return_type = column.type;
    }
  }
  // create the update node
  auto update = std::make_unique<LogicalUpdate>(
      table, column_ids, std::move(statement.expressions));
  update->AddChild(std::move(root));
  root = std::move(update);
}

void LogicalPlanGenerator::Visit(DeleteStatement& statement) {
  // we require row ids for the deletion
  require_row_id = true;
  // create the table scan
  statement.table->Accept(this);
  if (!root || root->type != LogicalOperatorType::GET) {
    throw Exception("Cannot create delete node without table scan!");
  }
  auto get = (LogicalGet*)root.get();
  // create the filter (if any)
  if (statement.condition) {
    statement.condition->Accept(this);
    auto filter =
        std::make_unique<LogicalFilter>(std::move(statement.condition));
    filter->AddChild(std::move(root));
    root = std::move(filter);
  }
  // create the delete node
  auto del = make_unique<LogicalDelete>(get->table);
  del->AddChild(std::move(root));
  root = std::move(del);
}

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
    filter->AddChild(std::move(root));
    root = std::move(filter);
  }

  if (statement.HasAggregation()) {
    auto aggregate =
        std::make_unique<LogicalAggregate>(std::move(statement.select_list));

    if (statement.HasGroup()) {
      // have to add group by columns
      aggregate->groups = std::move(statement.groupby.groups);
    }

    aggregate->AddChild(std::move(root));
    root = std::move(aggregate);

    if (statement.HasHaving()) {
      statement.groupby.having->Accept(this);

      auto having =
          std::make_unique<LogicalFilter>(std::move(statement.groupby.having));
      having->AddChild(std::move(root));
      root = std::move(having);
    }
  } else {
    auto projection =
        std::make_unique<LogicalProjection>(std::move(statement.select_list));
    projection->AddChild(std::move(root));
    root = std::move(projection);
  }

  if (statement.union_select) {
    auto top_select = std::move(root);
    statement.union_select->Accept(this);
    // TODO: LIMIT/ORDER BY with UNION? How does that work?
    auto bottom_select = std::move(root);
    if (!has_select_list(top_select->type) ||
        !has_select_list(bottom_select->type)) {
      throw Exception("UNION can only apply to projection, union or group");
    }
    if (top_select->expressions.size() != bottom_select->expressions.size()) {
      throw Exception(
          "UNION can only concatenate expressions with the "
          "same number of result column");
    }

    std::vector<std::unique_ptr<Expression>> expressions;
    for (size_t i = 0; i < top_select->expressions.size(); i++) {
      Expression* proj_ele = top_select->expressions[i].get();

      TypeId union_expr_type = TypeId::INVALID;
      auto top_expr_type = proj_ele->return_type;
      auto bottom_expr_type = bottom_select->expressions[i]->return_type;
      union_expr_type = top_expr_type;
      if (bottom_expr_type > union_expr_type) {
        union_expr_type = bottom_expr_type;
      }
      if (top_expr_type != union_expr_type) {
        auto cast = std::make_unique<CastExpression>(
            union_expr_type, std::move(top_select->expressions[i]));
        top_select->expressions[i] = std::move(cast);
      }
      if (bottom_expr_type != union_expr_type) {
        auto cast = std::make_unique<CastExpression>(
            union_expr_type, std::move(bottom_select->expressions[i]));
        bottom_select->expressions[i] = std::move(cast);
      }

      auto ele_ref =
          make_unique_base<Expression, ColumnRefExpression>(union_expr_type, i);
      ele_ref->alias = proj_ele->alias;
      expressions.push_back(std::move(ele_ref));
    }

    auto union_op = std::make_unique<LogicalUnion>(std::move(top_select),
                                                   std::move(bottom_select));
    union_op->expressions = std::move(expressions);

    root = std::move(union_op);
  }

  if (statement.select_distinct) {
    if (!has_select_list(root->type)) {
      throw Exception("DISTINCT can only apply to projection, union or group");
    }

    std::vector<std::unique_ptr<Expression>> expressions;
    std::vector<std::unique_ptr<Expression>> groups;

    for (size_t i = 0; i < root->expressions.size(); i++) {
      Expression* proj_ele = root->expressions[i].get();
      auto group_ref = make_unique_base<Expression, GroupRefExpression>(
          proj_ele->return_type, i);
      group_ref->alias = proj_ele->alias;
      expressions.push_back(std::move(group_ref));
      groups.push_back(make_unique_base<Expression, ColumnRefExpression>(
          proj_ele->return_type, i));
    }
    // this aggregate is superflous if all grouping columns are in aggr
    // below
    auto aggregate = std::make_unique<LogicalAggregate>(std::move(expressions));
    aggregate->groups = std::move(groups);
    aggregate->AddChild(std::move(root));
    root = std::move(aggregate);
  }

  if (statement.HasOrder()) {
    auto order = std::make_unique<LogicalOrder>(std::move(statement.orderby));
    order->AddChild(std::move(root));
    root = std::move(order);
  }
  if (statement.HasLimit()) {
    auto limit = std::make_unique<LogicalLimit>(statement.limit.limit,
                                                statement.limit.offset);
    limit->AddChild(std::move(root));
    root = std::move(limit);
  }
}

static void cast_children_to_equal_types(Expression& expr,
                                         size_t start_idx = 0) {
  // first figure out the widest type
  TypeId max_type = TypeId::INVALID;
  for (size_t child_idx = start_idx; child_idx < expr.children.size();
       child_idx++) {
    TypeId child_type = expr.children[child_idx]->return_type;
    if (child_type > max_type) {
      max_type = child_type;
    }
  }
  // now add casts where appropriate
  for (size_t child_idx = start_idx; child_idx < expr.children.size();
       child_idx++) {
    TypeId child_type = expr.children[child_idx]->return_type;
    if (child_type != max_type) {
      auto cast = std::make_unique<CastExpression>(
          max_type, std::move(expr.children[child_idx]));
      expr.children[child_idx] = std::move(cast);
    }
  }
}

void LogicalPlanGenerator::Visit(AggregateExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  // add cast if types don't match
  for (size_t i = 0; i < expr.children.size(); i++) {
    auto& child = expr.children[i];
    if (child->return_type != expr.return_type) {
      auto cast = std::make_unique<CastExpression>(expr.return_type,
                                                   std::move(expr.children[i]));
      expr.children[i] = std::move(cast);
    }
  }
}

void LogicalPlanGenerator::Visit(CaseExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  assert(expr.children.size() == 3);
  // check needs to be bool
  if (expr.children[0]->return_type != TypeId::BOOLEAN) {
    auto cast = std::make_unique<CastExpression>(TypeId::BOOLEAN,
                                                 std::move(expr.children[0]));
    expr.children[0] = std::move(cast);
  }
  // others need same type
  cast_children_to_equal_types(expr, 1);
}

void LogicalPlanGenerator::Visit(ComparisonExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  cast_children_to_equal_types(expr);
}

// TODO: this is ugly, generify functionality
void LogicalPlanGenerator::Visit(ConjunctionExpression& expr) {
  SQLNodeVisitor::Visit(expr);

  if (expr.children[0]->return_type != TypeId::BOOLEAN) {
    auto cast = std::make_unique<CastExpression>(TypeId::BOOLEAN,
                                                 std::move(expr.children[0]));
    expr.children[0] = std::move(cast);
  }
  if (expr.children[1]->return_type != TypeId::BOOLEAN) {
    auto cast = std::make_unique<CastExpression>(TypeId::BOOLEAN,
                                                 std::move(expr.children[1]));
    expr.children[1] = std::move(cast);
  }
}

void LogicalPlanGenerator::Visit(OperatorExpression& expr) {
  SQLNodeVisitor::Visit(expr);
  if (expr.type == ExpressionType::OPERATOR_NOT &&
      expr.children[0]->return_type != TypeId::BOOLEAN) {
    auto cast = std::make_unique<CastExpression>(TypeId::BOOLEAN,
                                                 std::move(expr.children[0]));
    expr.children[0] = std::move(cast);
  } else {
    cast_children_to_equal_types(expr);
  }
}

void LogicalPlanGenerator::Visit(SubqueryExpression& expr) {
  LogicalPlanGenerator generator(context, *expr.context);
  expr.subquery->Accept(&generator);
  if (!generator.root) {
    throw Exception("Can't plan subquery");
  }
  expr.op = std::move(generator.root);
}

void LogicalPlanGenerator::Visit(BaseTableRef& expr) {
  auto table = context.db.catalog.GetTable(context.ActiveTransaction(),
                                           expr.schema_name, expr.table_name);
  auto alias = expr.alias.empty() ? expr.table_name : expr.alias;

  auto index = bind_context.GetTableIndex(alias);

  std::vector<column_t> column_ids;
  // look in the context for this table which columns are required
  for (auto& bound_column : bind_context.bound_columns[alias]) {
    column_ids.push_back(table->name_map[bound_column]);
  }
  if (require_row_id || column_ids.size() == 0) {
    // no column ids selected
    // the query is like SELECT COUNT(*) FROM table, or SELECT 42 FROM table
    // return just the row id
    column_ids.push_back(COLUMN_IDENTIFIER_ROW_ID);
  }

  auto get_table =
      std::make_unique<LogicalGet>(table, alias, index, column_ids);
  if (root) {
    get_table->AddChild(std::move(root));
  }
  root = std::move(get_table);
}

void LogicalPlanGenerator::Visit(CrossProductRef& expr) {
  auto cross_product = std::make_unique<LogicalCrossProduct>();

  if (root) {
    throw Exception("Cross product cannot have children!");
  }

  expr.left->Accept(this);
  assert(root);
  cross_product->AddChild(std::move(root));
  root = nullptr;

  expr.right->Accept(this);
  assert(root);
  cross_product->AddChild(std::move(root));
  root = nullptr;

  root = std::move(cross_product);
}

void LogicalPlanGenerator::Visit(JoinRef& expr) {
  auto join = std::make_unique<LogicalJoin>(expr.type);

  if (root) {
    throw Exception("Cross product cannot have children!");
  }

  expr.left->Accept(this);
  assert(root);
  join->AddChild(std::move(root));
  root = nullptr;

  expr.right->Accept(this);
  assert(root);
  join->AddChild(std::move(root));
  root = nullptr;

  join->SetJoinCondition(std::move(expr.condition));
  if (join->conditions.size() > 0) {
    root = std::move(join);
  } else {  // the conditions were not comparisions between left on right,
            // cross product
    auto cp = make_unique<LogicalCrossProduct>();
    cp->AddChild(std::move(join->children[0]));
    cp->AddChild(std::move(join->children[1]));
    root = std::move(cp);
  }
}

void LogicalPlanGenerator::Visit(SubqueryRef& expr) {
  throw NotImplementedException("Joins not implemented yet!");
}

void LogicalPlanGenerator::Visit(InsertStatement& statement) {
  auto table = context.db.catalog.GetTable(context.ActiveTransaction(),
                                           statement.schema, statement.table);
  auto insert = std::make_unique<LogicalInsert>(table);

  if (statement.columns.size() > 0) {
    // insertion statement specifies column list

    // create a mapping of (list index) -> (column index)
    std::map<std::string, int> column_name_map;
    for (size_t i = 0; i < statement.columns.size(); i++) {
      column_name_map[statement.columns[i]] = i;
    }
    for (size_t i = 0; i < table->columns.size(); i++) {
      auto& col = table->columns[i];
      auto entry = column_name_map.find(col.name);
      if (entry == column_name_map.end()) {
        // column not specified, set index to -1
        insert->column_index_map.push_back(-1);
      } else {
        // column was specified, set to the index
        insert->column_index_map.push_back(entry->second);
      }
    }
  }

  if (statement.select_statement) {
    // insert from select statement
    // parse select statement and add to logical plan
    statement.select_statement->Accept(this);
    assert(root);
    insert->AddChild(std::move(root));
    root = std::move(insert);
  } else {
    // insert from constants
    // check if the correct amount of constants are supplied
    if (statement.columns.size() == 0) {
      if (statement.values[0].size() != table->columns.size()) {
        throw SyntaxException(
            "table %s has %d columns but %d values were supplied",
            table->name.c_str(), table->columns.size(),
            statement.values[0].size());
      }
    } else {
      if (statement.values[0].size() != statement.columns.size()) {
        throw SyntaxException(
            "Column name/value mismatch: %d values for %d columns",
            statement.values[0].size(), statement.columns.size());
      }
    }
    insert->insert_values = std::move(statement.values);
    root = std::move(insert);
  }
}

void LogicalPlanGenerator::Visit(CopyStatement& statement) {
  if (statement.table[0] != '\0') {
    auto table = context.db.catalog.GetTable(context.ActiveTransaction(),
                                             statement.schema, statement.table);
    auto copy = std::make_unique<LogicalCopy>(
        table, std::move(statement.file_path), std::move(statement.is_from),
        std::move(statement.delimiter), std::move(statement.quote),
        std::move(statement.escape), std::move(statement.select_list));
    root = std::move(copy);
  } else {
    auto copy = std::make_unique<LogicalCopy>(
        std::move(statement.file_path), std::move(statement.is_from),
        std::move(statement.delimiter), std::move(statement.quote),
        std::move(statement.escape));
    statement.select_statement->Accept(this);
    assert(root);
    copy->AddChild(std::move(root));
    root = std::move(copy);
  }
}

}  // namespace SummerDB