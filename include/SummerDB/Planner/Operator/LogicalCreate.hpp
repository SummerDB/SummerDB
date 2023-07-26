#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_CREATE_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_CREATE_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalCreate : public LogicalOperator {
 public:
  LogicalCreate(SchemaCatalogEntry* schema,
                std::unique_ptr<CreateTableInformation> info)
      : LogicalOperator(LogicalOperatorType::CREATE),
        schema(schema),
        info(std::move(info)) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  //! Schema to insert to
  SchemaCatalogEntry* schema;
  //! Create Table information
  std::unique_ptr<CreateTableInformation> info;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_CREATE_HPP