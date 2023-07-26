#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalGet represents a scan operation from a data source
class LogicalGet : public LogicalOperator {
 public:
  LogicalGet() : LogicalOperator(LogicalOperatorType::GET), table(nullptr) {}
  LogicalGet(TableCatalogEntry* table, std::string alias, size_t table_index,
             std::vector<column_t> column_ids)
      : LogicalOperator(LogicalOperatorType::GET),
        table(table),
        alias(alias),
        table_index(table_index),
        column_ids(column_ids) {
    referenced_tables.insert(table_index);
  }

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The base table to retrieve data from
  TableCatalogEntry* table;
  std::string alias;
  //! The table index in the current bind context
  size_t table_index;
  std::vector<column_t> column_ids;

  virtual std::string ParamsToString() const override {
    if (!table) {
      return "";
    }
    if (table->name == alias) {
      return "(" + alias + ")";
    }
    return "(" + table->name + " -> " + alias + ")";
  }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP