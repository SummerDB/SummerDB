#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

//! LogicalGet represents a scan operation from a data source
class LogicalGet : public LogicalOperator {
 public:
  LogicalGet() : LogicalOperator(LogicalOperatorType::GET) {}
  LogicalGet(std::shared_ptr<TableCatalogEntry> table, std::string alias)
      : LogicalOperator(LogicalOperatorType::GET), table(table), alias(alias) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! The base table to retrieve data from
  std::shared_ptr<TableCatalogEntry> table;
  std::string alias;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_GET_HPP