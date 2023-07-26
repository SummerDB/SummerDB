#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! PhysicalFilter represents a filter operator. It removes non-matching tupels
//! from the result. Note that it does not physically change the data, it only
//! adds a selection vector to the chunk.
class PhysicalFilter : public PhysicalOperator {
 public:
  PhysicalFilter(std::vector<std::unique_ptr<Expression>> select_list)
      : PhysicalOperator(PhysicalOperatorType::FILTER),
        expressions(std::move(select_list)) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent) override;

  std::vector<std::unique_ptr<Expression>> expressions;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_FILTER_HPP