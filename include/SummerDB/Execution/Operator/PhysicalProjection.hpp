#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

class PhysicalProjection : public PhysicalOperator {
 public:
  PhysicalProjection(std::vector<std::unique_ptr<Expression>> select_list)
      : PhysicalOperator(PhysicalOperatorType::PROJECTION),
        select_list(std::move(select_list)) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  std::vector<std::unique_ptr<Expression>> select_list;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP