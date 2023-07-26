#ifndef SUMMERDB_PLANNER_OPERATOR_LOGCIAL_DELETE_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGCIAL_DELETE_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalDelete : public LogicalOperator {
 public:
  LogicalDelete(TableCatalogEntry* table)
      : LogicalOperator(LogicalOperatorType::DELETE), table(table) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  TableCatalogEntry* table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGCIAL_DELETE_HPP