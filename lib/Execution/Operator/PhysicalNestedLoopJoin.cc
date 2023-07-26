#include "SummerDB/Execution/Operator/PhysicalNestedLoopJoin.hpp"

#include "SummerDB/Common/Types/VectorOperations.hpp"
#include "SummerDB/Execution/ExpressionExecutor.hpp"

namespace SummerDB {

PhysicalNestedLoopJoin::PhysicalNestedLoopJoin(
    std::unique_ptr<PhysicalOperator> left,
    std::unique_ptr<PhysicalOperator> right, std::vector<JoinCondition> cond,
    JoinType join_type)
    : PhysicalOperator(PhysicalOperatorType::NESTED_LOOP_JOIN),
      conditions(std::move(cond)),
      type(join_type) {
  children.push_back(std::move(left));
  children.push_back(std::move(right));
}

std::vector<TypeId> PhysicalNestedLoopJoin::GetTypes() {
  auto types = children[0]->GetTypes();
  auto right_types = children[1]->GetTypes();
  types.insert(types.end(), right_types.begin(), right_types.end());
  return types;
}

void PhysicalNestedLoopJoin::_GetChunk(ClientContext& context, DataChunk& chunk,
                                       PhysicalOperatorState* state_) {
  auto state = reinterpret_cast<PhysicalNestedLoopJoinOperatorState*>(state_);
  chunk.Reset();

  if (type != JoinType::INNER) {
    throw NotImplementedException("Only inner joins supported for now!");
  }

  // first we fully materialize the right child, if we haven't done that yet
  if (state->right_chunks.column_count() == 0) {
    auto right_state = children[1]->GetOperatorState(state->parent);
    auto types = children[1]->GetTypes();

    DataChunk new_chunk;
    new_chunk.Initialize(types);
    do {
      children[1]->GetChunk(context, new_chunk, right_state.get());
      state->right_chunks.Append(new_chunk);
    } while (new_chunk.count > 0);

    if (state->right_chunks.count == 0) {
      return;
    }
    // initialize the chunks for the join conditions
    std::vector<TypeId> left_types, right_types;
    for (auto& cond : conditions) {
      left_types.push_back(cond.left->return_type);
      right_types.push_back(cond.right->return_type);
    }
    state->left_join_condition.Initialize(left_types);
    state->right_join_condition.Initialize(right_types);
  }
  // now that we have fully materialized the right child
  // we have to perform the nested loop join

  do {
    chunk.Reset();
    // first check if we have to fetch a new chunk from the left child
    if (state->left_position >= state->child_chunk.count) {
      // if we have exhausted the current left chunk, fetch a new one
      children[0]->GetChunk(context, state->child_chunk,
                            state->child_state.get());
      if (state->child_chunk.count == 0) {
        return;
      }
      state->left_position = 0;
      state->right_chunk = 0;

      // resolve the left join condition for the current chunk
      state->left_join_condition.Reset();
      ExpressionExecutor executor(state->child_chunk, context);
      for (size_t i = 0; i < conditions.size(); i++) {
        executor.Execute(conditions[i].left.get(),
                         state->left_join_condition.data[i]);
      }
      state->left_join_condition.count =
          state->left_join_condition.data[0].count;
    }

    auto& left_chunk = state->child_chunk;
    auto& right_chunk = *state->right_chunks.chunks[state->right_chunk];

    // sanity check, this went wrong before
    left_chunk.Verify();
    right_chunk.Verify();

    // join the current row of the left relation with the current chunk
    // from the right relation
    state->right_join_condition.Reset();
    ExpressionExecutor executor(right_chunk, context);
    Vector final_result;
    for (size_t i = 0; i < conditions.size(); i++) {
      Vector& right_match = state->right_join_condition.data[i];
      // first resolve the join expression of the right side
      executor.Execute(conditions[i].right.get(), right_match);
      // now perform the join for the current tuple
      // we retrieve one value from the left hand side
      Vector left_match(
          state->left_join_condition.data[i].GetValue(state->left_position));

      Vector intermediate(TypeId::BOOLEAN, true, false);
      switch (conditions[i].comparison) {
        case ExpressionType::COMPARE_EQUAL:
          VectorOperations::Equals(left_match, right_match, intermediate);
          break;
        case ExpressionType::COMPARE_NOTEQUAL:
          VectorOperations::NotEquals(left_match, right_match, intermediate);
          break;
        case ExpressionType::COMPARE_LESSTHAN:
          VectorOperations::LessThan(left_match, right_match, intermediate);
          break;
        case ExpressionType::COMPARE_GREATERTHAN:
          VectorOperations::GreaterThan(left_match, right_match, intermediate);
          break;
        case ExpressionType::COMPARE_LESSTHANOREQUALTO:
          VectorOperations::LessThanEquals(left_match, right_match,
                                           intermediate);
          break;
        case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
          VectorOperations::GreaterThanEquals(left_match, right_match,
                                              intermediate);
          break;
        default:
          throw NotImplementedException(
              "Unsupported join comparison expression %s",
              ExpressionTypeToString(conditions[i].comparison).c_str());
      }
      if (i == 0) {
        // first predicate, move to the final result
        intermediate.Move(final_result);
      } else {
        // subsequent predicates: AND together
        VectorOperations::And(intermediate, final_result, final_result);
      }
    }
    assert(final_result.type == TypeId::BOOLEAN);
    // now we have the final result, create a selection vector from it
    chunk.SetSelectionVector(final_result);
    if (chunk.count > 0) {
      // we have elements in our join!
      // use the zero selection vector to prevent duplication on the left
      // side
      for (size_t i = 0; i < left_chunk.column_count; i++) {
        // first duplicate the values of the left side using the
        // selection vector
        chunk.data[i].count = chunk.count;
        VectorOperations::Set(
            chunk.data[i], left_chunk.data[i].GetValue(state->left_position));
      }
      // use the selection vector we created on the right side
      for (size_t i = 0; i < right_chunk.column_count; i++) {
        // now create a reference to the vectors of the right chunk
        size_t chunk_entry = left_chunk.column_count + i;
        chunk.data[chunk_entry].Reference(right_chunk.data[i]);
        chunk.data[chunk_entry].count = chunk.count;
        chunk.data[chunk_entry].sel_vector = chunk.sel_vector;
      }
    }

    state->right_chunk++;
    if (state->right_chunk >= state->right_chunks.chunks.size()) {
      // if we have exhausted all the chunks, move to the next tuple in
      // the left set
      state->left_position++;
      state->right_chunk = 0;
    }
  } while (chunk.count == 0);

  chunk.Verify();
}

std::unique_ptr<PhysicalOperatorState> PhysicalNestedLoopJoin::GetOperatorState(
    ExpressionExecutor* parent_executor) {
  return std::make_unique<PhysicalNestedLoopJoinOperatorState>(
      children[0].get(), children[1].get(), parent_executor);
}

}  // namespace SummerDB