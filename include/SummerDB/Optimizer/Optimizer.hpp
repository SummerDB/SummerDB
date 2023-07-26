#ifndef SUMMERDB_OPTIMIZER_OPTIMIZER_HPP
#define SUMMERDB_OPTIMIZER_OPTIMIZER_HPP

#include <string>
#include <vector>

#include "SummerDB/Planner/LogicalOperator.hpp"
#include "SummerDB/Planner/LogicalOperatorVisitor.hpp"

namespace SummerDB {

class Optimizer {
 public:
  Optimizer();

  std::unique_ptr<LogicalOperator> Optimize(
      std::unique_ptr<LogicalOperator> plan);

  bool GetSuccess() const { return success; }
  const std::string& GetErrorMessage() const { return message; }

  bool success;
  std::string message;
};

}  // namespace SummerDB

#endif  // SUMMERDB_OPTIMIZER_OPTIMIZER_HPP