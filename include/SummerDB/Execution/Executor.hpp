#ifndef SUMMERDB_EXECUTION_EXECUTOR_HPP
#define SUMMERDB_EXECUTION_EXECUTOR_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/SummerDB.hpp"

namespace SummerDB {

//! Executor is responsible for executing a physical operator plan and
//! outputting a result object
class Executor {
 public:
  //! Execute the specified physical operator plan
  std::unique_ptr<SummerDBResult> Execute(std::unique_ptr<PhysicalOperator> op);
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_EXECUTOR_HPP