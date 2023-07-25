#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalInsert represents an insertion of data into a base table
class LogicalInsert : public LogicalOperator {
 public:
  LogicalInsert(std::shared_ptr<TableCatalogEntry> table,
                std::vector<std::unique_ptr<AbstractExpression>> value_list)
      : LogicalOperator(LogicalOperatorType::INSERT),
        value_list(std::move(value_list)),
        table(table) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The set of values to insert into the table
  std::vector<std::unique_ptr<AbstractExpression>> value_list;
  //! The base table to insert into
  std::shared_ptr<TableCatalogEntry> table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_INSERT_HPP