#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UPDATE_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UPDATE_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically update data in a table
class PhysicalUpdate : public PhysicalOperator {
 public:
  PhysicalUpdate(DataTable& table, std::vector<column_t> columns,
                 std::vector<std::unique_ptr<Expression>> expressions)
      : PhysicalOperator(PhysicalOperatorType::UPDATE),
        table(table),
        columns(columns),
        expressions(std::move(expressions)) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  DataTable& table;
  std::vector<column_t> columns;
  std::vector<std::unique_ptr<Expression>> expressions;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_UPDATE_HPP