#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

class PhysicalProjection : public PhysicalOperator {
 public:
  PhysicalProjection(
      std::vector<std::unique_ptr<AbstractExpression>> select_list)
      : PhysicalOperator(PhysicalOperatorType::PROJECTION),
        select_list(move(select_list)) {}

  virtual void InitializeChunk(DataChunk& chunk) override;
  virtual void GetChunk(DataChunk& chunk,
                        PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState() override;

  std::vector<std::unique_ptr<AbstractExpression>> select_list;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_PROJECTION_HPP