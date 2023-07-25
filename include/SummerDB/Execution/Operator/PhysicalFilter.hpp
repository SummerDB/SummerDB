#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! PhysicalFilter represents a filter operator. It removes non-matching tupels
//! from the result. Note that it does not physically change the data, it only
//! adds a selection vector to the chunk.
class PhysicalFilter : public PhysicalOperator {
 public:
  PhysicalFilter(std::vector<std::unique_ptr<AbstractExpression>> select_list)
      : PhysicalOperator(PhysicalOperatorType::FILTER),
        expressions(std::move(select_list)) {}

  virtual void InitializeChunk(DataChunk& chunk) override;
  virtual void GetChunk(DataChunk& chunk,
                        PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState() override;

  std::vector<std::unique_ptr<AbstractExpression>> expressions;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP