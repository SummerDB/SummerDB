#ifndef SUMMERDB_EXECUTION_PHYSICAL_OPERATOR_HPP
#define SUMMERDB_EXECUTION_PHYSICAL_OPERATOR_HPP

#include <vector>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Common/Types/DataChunk.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"

namespace SummerDB {

class PhysicalOperator;

//! The current state/context of the operator. The PhysicalOperatorState is
//! updated using the GetChunk function, and allows the caller to repeatedly
//! call the GetChunk function and get new batches of data everytime until the
//! data source is exhausted.
class PhysicalOperatorState {
 public:
  PhysicalOperatorState(PhysicalOperator* child);
  virtual ~PhysicalOperatorState() {}

  //! Flag indicating whether or not the operator is finished [note: not all
  //! operators use this flag]
  bool finished;
  //! DataChunk that stores data from the child of this operator
  DataChunk child_chunk;
  //! State of the child of this operator
  std::unique_ptr<PhysicalOperatorState> child_state;
};

//! PhysicalOperator is the base class of the physical operators present in the
//! execution plan
/*!
    The execution model is a pull-based execution model. GetChunk is called on
   the root node, which causes the root node to be executed, and presumably call
   GetChunk again on its child nodes. Every node in the operator chain has a
   state that is updated as GetChunk is called: PhysicalOperatorState (different
   operators subclass this state and add different properties).
*/
class PhysicalOperator : public Printable {
 public:
  PhysicalOperator(PhysicalOperatorType type) : type(type) {}

  PhysicalOperatorType GetOperatorType() { return type; }

  virtual std::string ToString() const override;

  //! Initialize a given chunk to the types that will be returned by this
  //! operator, this will prepare chunk for a call to GetChunk. This method
  //! only has to be called once for any amount of calls to GetChunk.
  virtual void InitializeChunk(DataChunk& chunk) = 0;
  //! Retrieves a chunk from this operator and stores it in the chunk
  //! variable.
  virtual void GetChunk(DataChunk& chunk, PhysicalOperatorState* state) = 0;

  //! Create a new empty instance of the operator state
  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState() = 0;

  //! The physical operator type
  PhysicalOperatorType type;
  //! The set of children of the operator
  std::vector<std::unique_ptr<PhysicalOperator>> children;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_PHYSICAL_OPERATOR_HPP