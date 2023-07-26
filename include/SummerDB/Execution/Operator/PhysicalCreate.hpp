#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_CREATE_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_CREATE_HPP

#include <fstream>

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically CREATE TABLE statement
class PhysicalCreate : public PhysicalOperator {
 public:
  PhysicalCreate(SchemaCatalogEntry* schema,
                 std::unique_ptr<CreateTableInformation> info)
      : PhysicalOperator(PhysicalOperatorType::CREATE),
        schema(schema),
        info(std::move(info)) {}

  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* parent_executor) override;

  //! Schema to insert to
  SchemaCatalogEntry* schema;
  //! Table name to create
  std::unique_ptr<CreateTableInformation> info;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_CREATE_HPP