#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_NESTED_LOOP_JOIN_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_NESTED_LOOP_JOIN_HPP

#include "SummerDB/Common/Types/ChunkCollection.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Planner/Operator/LogicalJoin.hpp"

namespace SummerDB {

//! PhysicalNestedLoopJoin represents a nested loop join between two tables
class PhysicalNestedLoopJoin : public PhysicalOperator {
 public:
  PhysicalNestedLoopJoin(std::unique_ptr<PhysicalOperator> left,
                         std::unique_ptr<PhysicalOperator> right,
                         std::vector<JoinCondition> cond, JoinType join_type);

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  std::vector<JoinCondition> conditions;
  JoinType type;
};

class PhysicalNestedLoopJoinOperatorState : public PhysicalOperatorState {
 public:
  PhysicalNestedLoopJoinOperatorState(PhysicalOperator* left,
                                      PhysicalOperator* right,
                                      ExpressionExecutor* parent_executor)
      : PhysicalOperatorState(left, parent_executor),
        left_position(0),
        right_chunk(0) {
    assert(left && right);
  }

  size_t left_position;
  size_t right_chunk;
  DataChunk left_join_condition;
  DataChunk right_join_condition;
  ChunkCollection right_chunks;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_NESTED_LOOP_JOIN_HPP