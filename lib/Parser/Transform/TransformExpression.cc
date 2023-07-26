#include "SummerDB/Parser/Expression/HeaderList.hpp"
#include "SummerDB/Parser/Tableref/HeaderList.hpp"
#include "SummerDB/Parser/Transform.hpp"

namespace SummerDB {

using namespace postgres;

static bool IsAggregateFunction(const std::string& fun_name) {
  if (fun_name == "min" || fun_name == "max" || fun_name == "count" ||
      fun_name == "avg" || fun_name == "sum" || fun_name == "first")
    return true;
  return false;
}

std::string TransformAlias(Alias* root) {
  if (!root) {
    return "";
  }
  return root->aliasname;
}

TypeId TransformStringToTypeId(char* str) {
  std::string lower_str = StringUtil::Lower(std::string(str));
  // Transform column type
  if (lower_str == "int" || lower_str == "int4" || lower_str == "signed") {
    return TypeId::INTEGER;
  } else if (lower_str == "varchar" || lower_str == "bpchar" ||
             lower_str == "text" || lower_str == "string") {
    return TypeId::VARCHAR;
  } else if (lower_str == "int8") {
    return TypeId::BIGINT;
  } else if (lower_str == "int2") {
    return TypeId::SMALLINT;
  } else if (lower_str == "timestamp") {
    return TypeId::TIMESTAMP;
  } else if (lower_str == "bool") {
    return TypeId::BOOLEAN;
  } else if (lower_str == "double" || lower_str == "float8" ||
             lower_str == "real" || lower_str == "float4" ||
             lower_str == "numeric") {
    return TypeId::DECIMAL;
  } else if (lower_str == "tinyint") {
    return TypeId::TINYINT;
  } else if (lower_str == "varbinary") {
    return TypeId::VARBINARY;
  } else if (lower_str == "date") {
    return TypeId::DATE;
  } else {
    throw NotImplementedException("DataType %s not supported yet...\n", str);
  }
}

std::unique_ptr<Expression> TransformTypeCast(TypeCast* root) {
  if (!root) {
    return nullptr;
  }
  // get the type to cast to
  TypeName* type_name = root->typeName;
  char* name = (reinterpret_cast<value*>(type_name->names->tail->data.ptr_value)
                    ->val.str);
  TypeId target_type = TransformStringToTypeId(name);

  if (root->arg->type == T_A_Const) {
    // cast a constant value
    // get the original constant value
    auto constant = TransformConstant(reinterpret_cast<A_Const*>(root->arg));
    Value& source_value =
        reinterpret_cast<ConstantExpression*>(constant.get())->value;

    if (!source_value.is_null && TypeIsIntegral(source_value.type) &&
        TypeIsIntegral(target_type)) {
      // properly handle numeric overflows
      target_type =
          std::max(MinimalType(source_value.GetNumericValue()), target_type);
    }

    // perform the cast and substitute the expression
    Value new_value = source_value.CastAs(target_type);

    return std::make_unique<ConstantExpression>(new_value);
  } else {
    // transform the expression node
    auto expression = TransformExpression(root->arg);
    // now create a cast operation
    return std::make_unique<CastExpression>(target_type, std::move(expression));
  }
}

std::unique_ptr<Expression> TransformBoolExpr(BoolExpr* root) {
  std::unique_ptr<Expression> result;
  for (auto node = root->args->head; node != nullptr; node = node->next) {
    auto next =
        TransformExpression(reinterpret_cast<Node*>(node->data.ptr_value));

    switch (root->boolop) {
      case AND_EXPR: {
        if (!result) {
          result = std::move(next);
        } else {
          result = std::make_unique<ConjunctionExpression>(
              ExpressionType::CONJUNCTION_AND, std::move(result),
              std::move(next));
        }
        break;
      }
      case OR_EXPR: {
        if (!result) {
          result = std::move(next);
        } else {
          result = std::make_unique<ConjunctionExpression>(
              ExpressionType::CONJUNCTION_OR, std::move(result),
              std::move(next));
        }
        break;
      }
      case NOT_EXPR: {
        result = std::make_unique<OperatorExpression>(
            ExpressionType::OPERATOR_NOT, TypeId::BOOLEAN, std::move(next),
            nullptr);
        break;
      }
    }
  }
  return result;
}

std::unique_ptr<TableRef> TransformRangeVar(RangeVar* root) {
  auto result = std::make_unique<BaseTableRef>();

  result->alias = TransformAlias(root->alias);
  if (root->relname)
    result->table_name = root->relname;
  if (root->schemaname)
    result->schema_name = root->schemaname;
  if (root->catalogname)
    result->database_name = root->catalogname;
  return std::move(result);
}

std::unique_ptr<TableRef> TransformRangeSubselect(RangeSubselect* root) {
  auto result = make_unique<SubqueryRef>();
  result->alias = TransformAlias(root->alias);
  result->subquery = TransformSelect(root->subquery);
  if (!result->subquery) {
    return nullptr;
  }

  return std::move(result);
}

std::unique_ptr<TableRef> TransformJoin(JoinExpr* root) {
  auto result = std::make_unique<JoinRef>();
  switch (root->jointype) {
    case JOIN_INNER: {
      result->type = SummerDB::JoinType::INNER;
      break;
    }
    case JOIN_LEFT: {
      result->type = SummerDB::JoinType::LEFT;
      break;
    }
    case JOIN_FULL: {
      result->type = SummerDB::JoinType::OUTER;
      break;
    }
    case JOIN_RIGHT: {
      result->type = SummerDB::JoinType::RIGHT;
      break;
    }
    case JOIN_SEMI: {
      result->type = SummerDB::JoinType::SEMI;
      break;
    }
    default: {
      throw NotImplementedException("Join type %d not supported yet...\n",
                                    root->jointype);
    }
  }

  // Check the type of left arg and right arg before transform
  if (root->larg->type == T_RangeVar) {
    result->left = TransformRangeVar(reinterpret_cast<RangeVar*>(root->larg));
  } else if (root->larg->type == T_RangeSubselect) {
    result->left =
        TransformRangeSubselect(reinterpret_cast<RangeSubselect*>(root->larg));
  } else if (root->larg->type == T_JoinExpr) {
    result->left = TransformJoin(reinterpret_cast<JoinExpr*>(root->larg));
  } else {
    throw NotImplementedException("Join arg type %d not supported yet...\n",
                                  root->larg->type);
  }

  if (root->rarg->type == T_RangeVar) {
    result->right = TransformRangeVar(reinterpret_cast<RangeVar*>(root->rarg));
  } else if (root->rarg->type == T_RangeSubselect) {
    result->right =
        TransformRangeSubselect(reinterpret_cast<RangeSubselect*>(root->rarg));
  } else if (root->rarg->type == T_JoinExpr) {
    result->right = TransformJoin(reinterpret_cast<JoinExpr*>(root->rarg));
  } else {
    throw NotImplementedException("Join arg type %d not supported yet...\n",
                                  root->larg->type);
  }

  if (!root->quals) {  // CROSS JOIN
    auto cross = std::make_unique<CrossProductRef>();
    cross->left = std::move(result->left);
    cross->right = std::move(result->right);
    return std::move(cross);
  }

  // transform the quals, depends on AExprTranform and BoolExprTransform
  switch (root->quals->type) {
    case T_A_Expr: {
      result->condition =
          TransformAExpr(reinterpret_cast<A_Expr*>(root->quals));
      break;
    }
    case T_BoolExpr: {
      result->condition =
          TransformBoolExpr(reinterpret_cast<BoolExpr*>(root->quals));
      break;
    }
    default: {
      throw NotImplementedException("Join quals type %d not supported yet...\n",
                                    root->larg->type);
    }
  }
  return std::move(result);
}

std::unique_ptr<TableRef> TransformFrom(List* root) {
  if (!root) {
    return nullptr;
  }

  if (root->length > 1) {
    // Cross Product
    auto result = make_unique<CrossProductRef>();
    CrossProductRef* cur_root = result.get();
    for (auto node = root->head; node != nullptr; node = node->next) {
      std::unique_ptr<TableRef> next;
      Node* n = reinterpret_cast<Node*>(node->data.ptr_value);
      switch (n->type) {
        case T_RangeVar:
          next = TransformRangeVar(reinterpret_cast<RangeVar*>(n));
          break;
        case T_RangeSubselect:
          next = TransformRangeSubselect(reinterpret_cast<RangeSubselect*>(n));
          break;
        case T_JoinExpr:
          next = TransformJoin(reinterpret_cast<JoinExpr*>(n));
          break;
        default:
          throw NotImplementedException("From Type %d not supported yet...",
                                        n->type);
      }
      if (!cur_root->left) {
        cur_root->left = std::move(next);
      } else if (!cur_root->right) {
        cur_root->right = std::move(next);
      } else {
        auto old_res = std::move(result);
        result = std::make_unique<CrossProductRef>();
        result->left = std::move(old_res);
        result->right = std::move(next);
        cur_root = result.get();
      }
    }
    return std::move(result);
  }

  Node* n = reinterpret_cast<Node*>(root->head->data.ptr_value);
  switch (n->type) {
    case T_RangeVar: {
      return TransformRangeVar(reinterpret_cast<RangeVar*>(n));
    }
    case T_JoinExpr: {
      return TransformJoin(reinterpret_cast<JoinExpr*>(n));
    }
    case T_RangeSubselect: {
      return TransformRangeSubselect(reinterpret_cast<RangeSubselect*>(n));
    }
    default: {
      throw NotImplementedException("From Type %d not supported yet...",
                                    n->type);
    }
  }
}

std::unique_ptr<Expression> TransformColumnRef(ColumnRef* root) {
  List* fields = root->fields;
  switch ((reinterpret_cast<Node*>(fields->head->data.ptr_value))->type) {
    case T_String: {
      if (fields->length < 1 || fields->length > 2) {
        throw ParserException("Unexpected field length");
      }
      std::string column_name, table_name;
      if (fields->length == 1) {
        column_name = std::string(
            reinterpret_cast<value*>(fields->head->data.ptr_value)->val.str);
      } else {
        table_name = std::string(
            reinterpret_cast<value*>(fields->head->data.ptr_value)->val.str);
        column_name = std::string(
            reinterpret_cast<value*>(fields->head->next->data.ptr_value)
                ->val.str);
      }
      return std::make_unique<ColumnRefExpression>(column_name, table_name);
    }
    case T_A_Star: {
      return std::make_unique<ColumnRefExpression>();
    }
    default:
      throw NotImplementedException("ColumnRef not implemented!");
  }
}

std::unique_ptr<Expression> TransformValue(value val) {
  switch (val.type) {
    case T_Integer:
      return std::make_unique<ConstantExpression>(Value::INTEGER(val.val.ival));
    case T_BitString:  // FIXME: this should actually convert to BLOB
    case T_String:
      return std::make_unique<ConstantExpression>(
          Value(std::string(val.val.str)));
    case T_Float:
      return std::make_unique<ConstantExpression>(
          Value(std::stod(std::string(val.val.str))));
    case T_Null:
      return std::make_unique<ConstantExpression>();
    default:
      throw NotImplementedException("Value not implemented!");
  }
}

std::unique_ptr<Expression> TransformConstant(A_Const* c) {
  return TransformValue(c->val);
}

// COALESCE(a,b,c) returns the first argument that is NOT NULL, so
// rewrite into CASE(a IS NOT NULL, a, CASE(b IS NOT NULL, b, c))
std::unique_ptr<Expression> TransformCoalesce(A_Expr* root) {
  if (!root) {
    return nullptr;
  }
  auto coalesce_args = reinterpret_cast<List*>(root->lexpr);
  // TODO: this is somewhat duplicated from the CASE rewrite below, perhaps
  // they can be merged
  auto exp_root = std::unique_ptr<Expression>(new CaseExpression());
  Expression* cur_root = exp_root.get();
  Expression* next_root = nullptr;

  for (auto cell = coalesce_args->head; cell && cell->next; cell = cell->next) {
    // we need this twice
    auto value_expr =
        TransformExpression(reinterpret_cast<Node*>(cell->data.ptr_value));
    auto res_true =
        TransformExpression(reinterpret_cast<Node*>(cell->data.ptr_value));

    auto test = std::unique_ptr<Expression>(
        new OperatorExpression(ExpressionType::OPERATOR_IS_NOT_NULL,
                               TypeId::BOOLEAN, std::move(value_expr)));

    // the last argument does not need its own CASE because if we get there
    // we might as well return it directly
    std::unique_ptr<Expression> res_false;
    if (cell->next->next == nullptr) {
      res_false = TransformExpression(
          reinterpret_cast<Node*>(cell->next->data.ptr_value));
    } else {
      res_false = std::unique_ptr<Expression>(new CaseExpression());
      next_root = res_false.get();
    }
    cur_root->AddChild(std::move(test));
    cur_root->AddChild(std::move(res_true));
    cur_root->AddChild(std::move(res_false));
    cur_root = next_root;
  }
  return exp_root;
}

std::unique_ptr<Expression> TransformNullTest(NullTest* root) {
  if (!root) {
    return nullptr;
  }
  auto arg = TransformExpression(reinterpret_cast<Node*>(root->arg));
  if (root->argisrow) {
    throw NotImplementedException("IS NULL argisrow");
  }
  ExpressionType expr_type = (root->nulltesttype == IS_NULL)
                                 ? ExpressionType::OPERATOR_IS_NULL
                                 : ExpressionType::OPERATOR_IS_NOT_NULL;

  return std::unique_ptr<Expression>(
      new OperatorExpression(expr_type, TypeId::BOOLEAN, std::move(arg)));
}

std::unique_ptr<Expression> TransformAExpr(A_Expr* root) {
  if (!root) {
    return nullptr;
  }
  ExpressionType target_type;
  auto name = std::string(
      (reinterpret_cast<value*>(root->name->head->data.ptr_value))->val.str);

  switch (root->kind) {
    case AEXPR_DISTINCT:
      target_type = ExpressionType::COMPARE_DISTINCT_FROM;
      break;
    case AEXPR_IN: {
      auto left_expr = TransformExpression(root->lexpr);
      auto result = std::make_unique<OperatorExpression>(
          ExpressionType::COMPARE_IN, TypeId::BOOLEAN, std::move(left_expr));
      TransformExpressionList((List*)root->rexpr, result->children);
      // this looks very odd, but seems to be the way to find out its NOT IN
      if (name == "<>") {
        return std::make_unique<OperatorExpression>(
            ExpressionType::OPERATOR_NOT, TypeId::BOOLEAN, std::move(result),
            nullptr);
      } else {
        return std::move(result);
      }
    } break;
    // rewrite NULLIF(a, b) into CASE WHEN a=b THEN NULL ELSE a END
    case AEXPR_NULLIF: {
      auto case_expr = std::unique_ptr<Expression>(new CaseExpression());
      auto test_expr = std::unique_ptr<Expression>(new ComparisonExpression(
          ExpressionType::COMPARE_EQUAL, TransformExpression(root->lexpr),
          TransformExpression(root->rexpr)));
      case_expr->AddChild(std::move(test_expr));
      auto null_expr =
          std::unique_ptr<Expression>(new ConstantExpression(Value()));
      case_expr->AddChild(std::move(null_expr));
      case_expr->AddChild(TransformExpression(root->lexpr));
      return case_expr;
    } break;
    // rewrite (NOT) X BETWEEN A AND B into (NOT) AND(GREATERTHANOREQUALTO(X,
    // A), LESSTHANOREQUALTO(X, B))
    case AEXPR_BETWEEN:
    case AEXPR_NOT_BETWEEN: {
      auto between_args = reinterpret_cast<List*>(root->rexpr);

      if (between_args->length != 2 || !between_args->head->data.ptr_value ||
          !between_args->tail->data.ptr_value) {
        throw Exception("(NOT) BETWEEN needs two args");
      }

      auto between_left = TransformExpression(
          reinterpret_cast<Node*>(between_args->head->data.ptr_value));
      auto between_right = TransformExpression(
          reinterpret_cast<Node*>(between_args->tail->data.ptr_value));

      auto compare_left = std::make_unique<ComparisonExpression>(
          ExpressionType::COMPARE_GREATERTHANOREQUALTO,
          TransformExpression(root->lexpr), std::move(between_left));
      auto compare_right = std::make_unique<ComparisonExpression>(
          ExpressionType::COMPARE_LESSTHANOREQUALTO,
          TransformExpression(root->lexpr), std::move(between_right));
      auto compare_between = std::make_unique<ConjunctionExpression>(
          ExpressionType::CONJUNCTION_AND, std::move(compare_left),
          std::move(compare_right));
      if (root->kind == AEXPR_BETWEEN) {
        return std::move(compare_between);
      } else {
        return std::make_unique<OperatorExpression>(
            ExpressionType::OPERATOR_NOT, TypeId::BOOLEAN,
            std::move(compare_between), nullptr);
      }
    } break;
    default: {
      target_type = StringToExpressionType(name);
      if (target_type == ExpressionType::INVALID) {
        throw NotImplementedException("A_Expr transform not implemented %s.",
                                      name.c_str());
      }
    }
  }

  // continuing default case
  auto left_expr = TransformExpression(root->lexpr);
  auto right_expr = TransformExpression(root->rexpr);
  if (!left_expr) {
    switch (target_type) {
      case ExpressionType::OPERATOR_ADD:
        return right_expr;
      case ExpressionType::OPERATOR_SUBTRACT:
        target_type = ExpressionType::OPERATOR_MULTIPLY;
        left_expr = std::make_unique<ConstantExpression>(Value(-1));
        break;
      default:
        throw Exception("Unknown unary operator");
    }
  }

  std::unique_ptr<Expression> result = nullptr;
  int type_id = static_cast<int>(target_type);
  if (type_id <= 6) {
    result = std::make_unique<OperatorExpression>(target_type, TypeId::INVALID,
                                                  std::move(left_expr),
                                                  std::move(right_expr));
  } else if (((10 <= type_id) && (type_id <= 17)) || (type_id == 20)) {
    result = std::make_unique<ComparisonExpression>(
        target_type, std::move(left_expr), std::move(right_expr));
  } else {
    throw NotImplementedException("A_Expr transform not implemented.");
  }
  return result;
}

std::unique_ptr<Expression> TransformFuncCall(FuncCall* root) {
  std::string fun_name = StringUtil::Lower(
      (reinterpret_cast<value*>(root->funcname->head->data.ptr_value))
          ->val.str);

  if (!IsAggregateFunction(fun_name)) {
    // Normal functions (i.e. built-in functions or UDFs)
    fun_name = (reinterpret_cast<value*>(root->funcname->tail->data.ptr_value))
                   ->val.str;
    std::vector<std::unique_ptr<Expression>> children;
    if (root->args != nullptr) {
      for (auto node = root->args->head; node != nullptr; node = node->next) {
        auto child_expr = TransformExpression((Node*)node->data.ptr_value);
        children.push_back(std::move(child_expr));
      }
    }
    return std::make_unique<FunctionExpression>(fun_name.c_str(), children);
  } else {
    // Aggregate function
    auto agg_fun_type = StringToExpressionType("AGGREGATE_" + fun_name);
    if (root->agg_star) {
      return std::make_unique<AggregateExpression>(
          agg_fun_type, false, std::make_unique<ColumnRefExpression>());
    } else {
      if (!root->args) {
        throw NotImplementedException(
            "Aggregation over zero columns not supported!");
      } else if (root->args->length < 2) {
        if (agg_fun_type == ExpressionType::AGGREGATE_AVG) {
          // rewrite AVG(a) to SUM(a) / COUNT(a)
          // first create the SUM
          auto sum = std::make_unique<AggregateExpression>(
              ExpressionType::AGGREGATE_SUM, root->agg_distinct,
              TransformExpression((Node*)root->args->head->data.ptr_value));
          // now create the count
          auto count = std::make_unique<AggregateExpression>(
              ExpressionType::AGGREGATE_COUNT, root->agg_distinct,
              TransformExpression((Node*)root->args->head->data.ptr_value));
          // cast both to decimal
          auto sum_cast =
              std::make_unique<CastExpression>(TypeId::DECIMAL, std::move(sum));
          auto count_cast = std::make_unique<CastExpression>(TypeId::DECIMAL,
                                                             std::move(count));
          // create the divide operator
          return std::make_unique<OperatorExpression>(
              ExpressionType::OPERATOR_DIVIDE, TypeId::DECIMAL,
              std::move(sum_cast), std::move(count_cast));
        } else {
          auto child =
              TransformExpression((Node*)root->args->head->data.ptr_value);
          return std::make_unique<AggregateExpression>(
              agg_fun_type, root->agg_distinct, std::move(child));
        }
      } else {
        throw NotImplementedException(
            "Aggregation over multiple columns not supported yet...\n");
      }
    }
  }
}

std::unique_ptr<Expression> TransformCase(CaseExpr* root) {
  if (!root) {
    return nullptr;
  }
  // CASE expression WHEN value THEN result [WHEN ...] ELSE result uses this,
  // but we rewrite to CASE WHEN expression = value THEN result ... to only
  // have to handle one case downstream.

  std::unique_ptr<Expression> def_res;
  if (root->defresult) {
    def_res = TransformExpression(reinterpret_cast<Node*>(root->defresult));
  } else {
    def_res = std::unique_ptr<Expression>(new ConstantExpression(Value()));
  }
  // def_res will be the else part of the innermost case expression

  // CASE WHEN e1 THEN r1 WHEN w2 THEN r2 ELSE r3 is rewritten to
  // CASE WHEN e1 THEN r1 ELSE CASE WHEN e2 THEN r2 ELSE r3

  auto exp_root = std::unique_ptr<Expression>(new CaseExpression());
  Expression* cur_root = exp_root.get();
  Expression* next_root = nullptr;

  for (auto cell = root->args->head; cell != nullptr; cell = cell->next) {
    CaseWhen* w = reinterpret_cast<CaseWhen*>(cell->data.ptr_value);

    auto test_raw = TransformExpression(reinterpret_cast<Node*>(w->expr));
    std::unique_ptr<Expression> test;
    // TODO: how do we copy those things?
    auto arg = TransformExpression(reinterpret_cast<Node*>(root->arg));

    if (arg) {
      test = std::unique_ptr<Expression>(new ComparisonExpression(
          ExpressionType::COMPARE_EQUAL, std::move(arg), std::move(test_raw)));
    } else {
      test = std::move(test_raw);
    }

    auto res_true = TransformExpression(reinterpret_cast<Node*>(w->result));

    std::unique_ptr<Expression> res_false;
    if (cell->next == nullptr) {
      res_false = std::move(def_res);
    } else {
      res_false = std::unique_ptr<Expression>(new CaseExpression());
      next_root = res_false.get();
    }

    cur_root->AddChild(std::move(test));
    cur_root->AddChild(std::move(res_true));
    cur_root->AddChild(std::move(res_false));

    cur_root = next_root;
  }

  return exp_root;
}

std::unique_ptr<Expression> TransformSubquery(SubLink* root) {
  if (!root) {
    return nullptr;
  }
  auto subquery_expr = std::make_unique<SubqueryExpression>();
  subquery_expr->subquery = TransformSelect(root->subselect);
  if (!subquery_expr->subquery) {
    return nullptr;
  }

  switch (root->subLinkType) {
    case EXISTS_SUBLINK: {
      subquery_expr->type = SubqueryType::EXISTS;
      return std::make_unique<OperatorExpression>(
          ExpressionType::OPERATOR_EXISTS, TypeId::BOOLEAN,
          std::move(subquery_expr));
    }
    case ANY_SUBLINK: {
      subquery_expr->type = SubqueryType::IN;
      return std::make_unique<OperatorExpression>(
          ExpressionType::COMPARE_IN, TypeId::BOOLEAN,
          TransformExpression(root->testexpr), std::move(subquery_expr));
    }
    case EXPR_SUBLINK: {
      return subquery_expr;
    }
    default: {
      throw NotImplementedException("Subquery of type %d not implemented\n",
                                    (int)root->subLinkType);
    }
  }
}

std::unique_ptr<Expression> TransformResTarget(ResTarget* root) {
  if (!root) {
    return nullptr;
  }
  auto expr = TransformExpression(root->val);
  if (!expr) {
    return nullptr;
  }
  if (root->name) {
    expr->alias = std::string(root->name);
  }
  return expr;
}

std::unique_ptr<Expression> TransformExpression(Node* node) {
  if (!node) {
    return nullptr;
  }

  switch (node->type) {
    case T_ColumnRef:
      return TransformColumnRef(reinterpret_cast<ColumnRef*>(node));
    case T_A_Const:
      return TransformConstant(reinterpret_cast<A_Const*>(node));
    case T_A_Expr:
      return TransformAExpr(reinterpret_cast<A_Expr*>(node));
    case T_FuncCall:
      return TransformFuncCall(reinterpret_cast<FuncCall*>(node));
    case T_BoolExpr:
      return TransformBoolExpr(reinterpret_cast<BoolExpr*>(node));
    case T_TypeCast:
      return TransformTypeCast(reinterpret_cast<TypeCast*>(node));
    case T_CaseExpr:
      return TransformCase(reinterpret_cast<CaseExpr*>(node));
    case T_SubLink:
      return TransformSubquery(reinterpret_cast<SubLink*>(node));
    case T_CoalesceExpr:
      return TransformCoalesce(reinterpret_cast<A_Expr*>(node));
    case T_NullTest:
      return TransformNullTest(reinterpret_cast<NullTest*>(node));
    case T_ResTarget:
      return TransformResTarget(reinterpret_cast<ResTarget*>(node));
    case T_SetToDefault:
      return make_unique<DefaultExpression>();
    case T_ParamRef:

    default:
      throw NotImplementedException("Expr of type %d not implemented\n",
                                    (int)node->type);
  }
}

bool TransformExpressionList(List* list,
                             std::vector<std::unique_ptr<Expression>>& result) {
  if (!list) {
    return false;
  }
  for (auto node = list->head; node != nullptr; node = node->next) {
    auto target = reinterpret_cast<Node*>(node->data.ptr_value);
    if (!target) {
      return false;
    }
    auto expr = TransformExpression(target);
    if (!expr) {
      return false;
    }
    result.push_back(std::move(expr));
  }
  return true;
}

std::unique_ptr<Expression> TransformListValue(Expr* node) {
  if (!node) {
    return nullptr;
  }

  switch (node->type) {
    case T_A_Const:
      return TransformConstant((A_Const*)node);
    case T_ParamRef:
    case T_TypeCast:
    case T_SetToDefault:

    default:
      throw NotImplementedException("Expr of type %d not implemented\n",
                                    (int)node->type);
  }
}

}  // namespace SummerDB