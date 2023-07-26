#include "SummerDB/Planner/Operator/LogicalJoin.hpp"

#include "SummerDB/Parser/Expression/HeaderList.hpp"
#include "SummerDB/Planner/Operator/LogicalFilter.hpp"

namespace SummerDB {

JoinSide LogicalJoin::GetJoinSide(LogicalOperator* op,
                                  std::unique_ptr<Expression>& expr) {
  if (expr->type == ExpressionType::COLUMN_REF) {
    auto colref = (ColumnRefExpression*)expr.get();
    if (op->children[0]->referenced_tables.find(colref->binding.table_index) !=
        op->children[0]->referenced_tables.end()) {
      return JoinSide::LEFT;
    } else {
      if (op->children[1]->referenced_tables.find(
              colref->binding.table_index) !=
          op->children[1]->referenced_tables.end()) {
        return JoinSide::RIGHT;
      } else {
        return JoinSide::NONE;
      }
    }
  } else {
    JoinSide join_side = JoinSide::NONE;
    for (auto& child : expr->children) {
      auto child_side = LogicalJoin::GetJoinSide(op, child);
      if (child_side != join_side) {
        join_side = join_side == JoinSide::NONE ? child_side : JoinSide::BOTH;
      }
    }
    return join_side;
  }
}

static ExpressionType NegateComparisionExpression(ExpressionType type) {
  ExpressionType negated_type = ExpressionType::INVALID;
  switch (type) {
    case ExpressionType::COMPARE_EQUAL:
      negated_type = ExpressionType::COMPARE_NOTEQUAL;
      break;
    case ExpressionType::COMPARE_NOTEQUAL:
      negated_type = ExpressionType::COMPARE_EQUAL;
      break;
    case ExpressionType::COMPARE_LESSTHAN:
      negated_type = ExpressionType::COMPARE_GREATERTHANOREQUALTO;
      break;
    case ExpressionType::COMPARE_GREATERTHAN:
      negated_type = ExpressionType::COMPARE_LESSTHANOREQUALTO;
      break;
    case ExpressionType::COMPARE_LESSTHANOREQUALTO:
      negated_type = ExpressionType::COMPARE_GREATERTHAN;
      break;
    case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
      negated_type = ExpressionType::COMPARE_LESSTHAN;
      break;

    default:
      throw Exception("Unsupported join criteria in negation");
  }
  return negated_type;
}

static ExpressionType FlipComparisionExpression(ExpressionType type) {
  ExpressionType flipped_type = ExpressionType::INVALID;
  switch (type) {
    case ExpressionType::COMPARE_NOTEQUAL:
    case ExpressionType::COMPARE_EQUAL:
      flipped_type = type;
      break;
    case ExpressionType::COMPARE_LESSTHAN:
      flipped_type = ExpressionType::COMPARE_GREATERTHAN;
      break;
    case ExpressionType::COMPARE_GREATERTHAN:
      flipped_type = ExpressionType::COMPARE_LESSTHAN;
      break;
    case ExpressionType::COMPARE_LESSTHANOREQUALTO:
      flipped_type = ExpressionType::COMPARE_GREATERTHANOREQUALTO;
      break;
    case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
      flipped_type = ExpressionType::COMPARE_LESSTHANOREQUALTO;
      break;

    default:
      throw Exception("Unsupported join criteria in flip");
  }
  return flipped_type;
}

void LogicalJoin::SetJoinCondition(std::unique_ptr<Expression> condition) {
  assert(children.size() == 2);
  if (condition->GetExpressionType() == ExpressionType::CONJUNCTION_AND) {
    // traverse down the expression tree along conjunction
    for (auto& child : condition->children) {
      SetJoinCondition(std::move(child));
    }
  } else if (condition->GetExpressionType() >= ExpressionType::COMPARE_EQUAL &&
             condition->GetExpressionType() <=
                 ExpressionType::COMPARE_GREATERTHANOREQUALTO) {
    // logical comparison
    // figure out which side belongs to the left and which side belongs to
    // the right
    assert(condition->children.size() == 2);
    size_t left_side = LogicalJoin::GetJoinSide(this, condition->children[0]);
    size_t right_side = LogicalJoin::GetJoinSide(this, condition->children[1]);

    JoinCondition join_condition;
    join_condition.comparison = condition->GetExpressionType();
    if (left_side == JoinSide::LEFT && right_side == JoinSide::RIGHT) {
      // left is left right is right
      join_condition.left = std::move(condition->children[0]);
      join_condition.right = std::move(condition->children[1]);
    } else if (left_side == JoinSide::RIGHT && right_side == JoinSide::LEFT) {
      // left is right right is left
      join_condition.left = std::move(condition->children[1]);
      join_condition.right = std::move(condition->children[0]);
      // have to negate the condition, too
      join_condition.comparison =
          FlipComparisionExpression(join_condition.comparison);
    } else {
      // create filters for non-comparision conditions
      auto filter = std::make_unique<LogicalFilter>(std::move(condition));
      if (left_side == JoinSide::LEFT || right_side == JoinSide::LEFT) {
        filter->AddChild(std::move(children[0]));
        children[0] = std::move(filter);
      } else {
        // if neither side is part of the join we push to right side as
        // well because whatever
        filter->AddChild(std::move(children[1]));
        children[1] = std::move(filter);
      }
      return;
    }
    conditions.push_back(std::move(join_condition));

  } else if (condition->GetExpressionType() == ExpressionType::OPERATOR_NOT) {
    assert(condition->children.size() == 1);
    ExpressionType child_type = condition->children[0]->GetExpressionType();

    if (child_type < ExpressionType::COMPARE_EQUAL ||
        child_type > ExpressionType::COMPARE_GREATERTHANOREQUALTO) {
      throw Exception("ON NOT only supports comparision operators");
    }
    // switcheroo the child condition
    // our join needs to compare explicit left and right sides. So we invert
    // the condition to express NOT, this way we can still use equi-joins

    condition->children[0]->type = NegateComparisionExpression(child_type);
    SetJoinCondition(std::move(condition->children[0]));
  }
}

}  // namespace SummerDB