#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DELETE_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DELETE_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically delete data from a table
class PhysicalDelete : public PhysicalOperator {
 public:
  PhysicalDelete(DataTable& table)
      : PhysicalOperator(PhysicalOperatorType::DELETE), table(table) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  DataTable& table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_DELETE_HPP