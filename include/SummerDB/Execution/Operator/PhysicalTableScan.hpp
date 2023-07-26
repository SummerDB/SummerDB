#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_TABLE_SCAN_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_TABLE_SCAN_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Storage/DataTable.hpp"

namespace SummerDB {

//! Represents a scan of a base table
class PhysicalTableScan : public PhysicalOperator {
 public:
  PhysicalTableScan(DataTable& table, std::vector<column_t> column_ids)
      : PhysicalOperator(PhysicalOperatorType::SEQ_SCAN),
        table(table),
        column_ids(column_ids) {}

  DataTable& table;
  std::vector<column_t> column_ids;

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;
};

class PhysicalTableScanOperatorState : public PhysicalOperatorState {
 public:
  PhysicalTableScanOperatorState(DataTable& table,
                                 ExpressionExecutor* parent_executor)
      : PhysicalOperatorState(nullptr, parent_executor) {
    table.InitializeScan(scan_offset);
  }

  //! The current position in the scan
  ScanStructure scan_offset;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_TABLE_SCAN_HPP