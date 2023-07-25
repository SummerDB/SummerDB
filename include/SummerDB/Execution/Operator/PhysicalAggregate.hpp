#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_AGGREGATE_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_AGGREGATE_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! PhysicalAggregate represents a group-by and aggregation operator. Note that
//! it is an abstract class, its implementation is not defined here.
class PhysicalAggregate : public PhysicalOperator {
 public:
  PhysicalAggregate(
      std::vector<std::unique_ptr<AbstractExpression>> select_list,
      PhysicalOperatorType type = PhysicalOperatorType::BASE_GROUP_BY);
  PhysicalAggregate(
      std::vector<std::unique_ptr<AbstractExpression>> select_list,
      std::vector<std::unique_ptr<AbstractExpression>> groups,
      PhysicalOperatorType type = PhysicalOperatorType::BASE_GROUP_BY);

  void Initialize();

  void InitializeChunk(DataChunk& chunk) override;

  //! The projection list of the SELECT statement (that contains aggregates)
  std::vector<std::unique_ptr<AbstractExpression>> select_list;
  //! The groups
  std::vector<std::unique_ptr<AbstractExpression>> groups;
  //! The actual aggregates that have to be computed (i.e. the deepest
  //! aggregates in the expression)
  std::vector<AggregateExpression*> aggregates;
};

//! The operator state of the aggregate
class PhysicalAggregateOperatorState : public PhysicalOperatorState {
 public:
  PhysicalAggregateOperatorState(PhysicalAggregate* parent,
                                 PhysicalOperator* child = nullptr);

  //! Aggregate values, used only for aggregates without GROUP BY
  std::vector<Value> aggregates;
  //! Materialized GROUP BY expression
  DataChunk group_chunk;
  //! Materialized aggregates
  DataChunk aggregate_chunk;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_AGGREGATE_HPP