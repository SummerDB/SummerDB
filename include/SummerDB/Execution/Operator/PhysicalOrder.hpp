#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_ORDER_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_ORDER_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Represents a physical ordering of the data. Note that this will not change
//! the data but only add a selection vector.
class PhysicalOrder : public PhysicalOperator {
 public:
  PhysicalOrder(OrderByDescription description)
      : PhysicalOperator(PhysicalOperatorType::ORDER_BY),
        description(std::move(description)) {}

  virtual void InitializeChunk(DataChunk& chunk) override;
  virtual void GetChunk(DataChunk& chunk,
                        PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState() override;

  OrderByDescription description;
};

class PhysicalOrderOperatorState : public PhysicalOperatorState {
 public:
  PhysicalOrderOperatorState(PhysicalOperator* child)
      : PhysicalOperatorState(child), position(0) {}

  size_t position;
  DataChunk sorted_data;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_ORDER_HPP