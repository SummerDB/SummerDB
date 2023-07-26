#ifndef SUMMERDB_EXECUTION_EXECUTOR_HPP
#define SUMMERDB_EXECUTION_EXECUTOR_HPP

#include "SummerDB/Common/Types/ChunkCollection.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Executor is responsible for executing a physical operator plan and
//! outputting a result object
class Executor {
 public:
  //! Execute the specified physical operator plan
  ChunkCollection Execute(ClientContext& context,
                          std::unique_ptr<PhysicalOperator> op);
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_EXECUTOR_HPP