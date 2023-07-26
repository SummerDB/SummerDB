#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically insert a set of data into a table
class PhysicalInsert : public PhysicalOperator {
 public:
  PhysicalInsert(
      TableCatalogEntry* table,
      std::vector<std::vector<std::unique_ptr<Expression>>> insert_values,
      std::vector<int> column_index_map)
      : PhysicalOperator(PhysicalOperatorType::INSERT),
        column_index_map(column_index_map),
        insert_values(std::move(insert_values)),
        table(table) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  std::vector<int> column_index_map;
  std::vector<std::vector<std::unique_ptr<Expression>>> insert_values;
  TableCatalogEntry* table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP