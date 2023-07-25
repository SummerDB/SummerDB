#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically insert a set of data into a table
class PhysicalInsert : public PhysicalOperator {
 public:
  PhysicalInsert(std::shared_ptr<TableCatalogEntry> table,
                 std::vector<std::unique_ptr<AbstractExpression>> value_list)
      : PhysicalOperator(PhysicalOperatorType::INSERT),
        value_list(move(value_list)),
        table(table) {}

  virtual void InitializeChunk(DataChunk& chunk) override;
  virtual void GetChunk(DataChunk& chunk,
                        PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState() override;

  std::vector<std::unique_ptr<AbstractExpression>> value_list;
  std::shared_ptr<TableCatalogEntry> table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_INSERT_HPP