#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalInsert represents an insertion of data into a base table
class LogicalInsert : public LogicalOperator {
 public:
  LogicalInsert(TableCatalogEntry* table)
      : LogicalOperator(LogicalOperatorType::INSERT), table(table) {}

  std::vector<std::vector<std::unique_ptr<Expression>>> insert_values;
  std::vector<int> column_index_map;

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The base table to insert into
  TableCatalogEntry* table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP