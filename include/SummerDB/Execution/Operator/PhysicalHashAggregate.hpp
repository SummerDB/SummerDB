#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_HASH_AGGREGATE_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_HASH_AGGREGATE_HPP

#include "SummerDB/Execution/AggregateHashtable.hpp"
#include "SummerDB/Execution/Operator/PhysicalAggregate.hpp"
#include "SummerDB/Storage/DataTable.hpp"

namespace SummerDB {

//! PhysicalHashAggregate is an group-by and aggregate implementation that uses
//! a hash table to perform the grouping
class PhysicalHashAggregate : public PhysicalAggregate {
 public:
  PhysicalHashAggregate(
      std::vector<std::unique_ptr<AbstractExpression>> expressions);
  PhysicalHashAggregate(
      std::vector<std::unique_ptr<AbstractExpression>> expressions,
      std::vector<std::unique_ptr<AbstractExpression>> groups);

  void Initialize();

  void InitializeChunk(DataChunk& chunk) override;
  void GetChunk(DataChunk& chunk, PhysicalOperatorState* state) override;

  std::unique_ptr<PhysicalOperatorState> GetOperatorState() override;
};

class PhysicalHashAggregateOperatorState
    : public PhysicalAggregateOperatorState {
 public:
  PhysicalHashAggregateOperatorState(PhysicalAggregate* parent,
                                     PhysicalOperator* child)
      : PhysicalAggregateOperatorState(parent, child), ht_scan_position(0) {}

  //! The current position to scan the HT for output tuples
  size_t ht_scan_position;
  //! The HT
  std::unique_ptr<SuperLargeHashTable> ht;
  //! The payload chunk, only used while filling the HT
  DataChunk payload_chunk;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_HASH_AGGREGATE_HPP