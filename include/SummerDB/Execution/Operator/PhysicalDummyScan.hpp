#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DUMMY_SCAN_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DUMMY_SCAN_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

class PhysicalDummyScan : public PhysicalOperator {
 public:
  PhysicalDummyScan() : PhysicalOperator(PhysicalOperatorType::DUMMY_SCAN) {}

  // we can hard-code some data into this scan if req
  DataChunk chunk;

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DUMMY_SCAN_HPP