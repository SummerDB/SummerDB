#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UNION_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UNION_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

class PhysicalUnion : public PhysicalOperator {
 public:
  PhysicalUnion(std::unique_ptr<PhysicalOperator> top,
                std::unique_ptr<PhysicalOperator> bottom);

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;
  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;
};

class PhysicalUnionOperatorState : public PhysicalOperatorState {
 public:
  PhysicalUnionOperatorState(ExpressionExecutor* parent_executor)
      : PhysicalOperatorState(nullptr, parent_executor), top_done(false) {}
  std::unique_ptr<PhysicalOperatorState> top_state;
  std::unique_ptr<PhysicalOperatorState> bottom_state;
  bool top_done = false;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UNION_HPP