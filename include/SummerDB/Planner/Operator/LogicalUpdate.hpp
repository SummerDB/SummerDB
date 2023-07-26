#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_UPDATE_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_UPDATE_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalUpdate : public LogicalOperator {
 public:
  LogicalUpdate(TableCatalogEntry* table, std::vector<column_t> columns,
                std::vector<std::unique_ptr<Expression>> expressions)
      : LogicalOperator(LogicalOperatorType::UPDATE, std::move(expressions)),
        table(table),
        columns(columns) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  TableCatalogEntry* table;
  std::vector<column_t> columns;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_UPDATE_HPP