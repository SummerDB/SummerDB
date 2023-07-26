#ifndef SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP
#define SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Common/Types/DataChunk.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

class ClientContext;

//! ExpressionExecutor is responsible for executing an arbitrary
//! Expression and returning a Vector
/*!
    ExpressionExecutor is responsible for executing an arbitrary
   Expression and returning a Vector. It executes the expressions
   recursively using a visitor pattern.
*/
class ExpressionExecutor : public SQLNodeVisitor {
 public:
  ExpressionExecutor(PhysicalOperatorState* state, ClientContext& context,
                     bool scalar_executor = true)
      : context(context),
        scalar_executor(scalar_executor),
        chunk(state ? &state->child_chunk : nullptr),
        parent(state ? state->parent : nullptr),
        state(state) {}

  ExpressionExecutor(DataChunk& child_chunk, ClientContext& context,
                     ExpressionExecutor* parent = nullptr)
      : context(context),
        scalar_executor(true),
        chunk(&child_chunk),
        parent(parent),
        state(nullptr) {}

  void Reset();

  //! Execute a single abstract expression and store the result in result
  void Execute(Expression* expr, Vector& result);
  //! Execute the abstract expression, and "logical AND" the result together
  //! with result
  void Merge(Expression* expr, Vector& result);
  //! Execute the given aggregate expression, and merge the result together
  //! with v
  void Merge(AggregateExpression& expr, Value& v);
  //! Execute the given aggregate expression for the current chunk
  Value Execute(AggregateExpression& expr);

  void Visit(AggregateExpression& expr);
  void Visit(CaseExpression& expr);
  void Visit(CastExpression& expr);
  void Visit(ColumnRefExpression& expr);
  void Visit(ComparisonExpression& expr);
  void Visit(ConjunctionExpression& expr);
  void Visit(ConstantExpression& expr);
  void Visit(FunctionExpression& expr);
  void Visit(GroupRefExpression& expr);
  void Visit(OperatorExpression& expr);
  void Visit(SubqueryExpression& expr);

 private:
  ClientContext& context;

  //! Whether or not the ExpressionExecutor is a scalar executor (i.e. output
  //! size = input size), this is true for e.g. expressions in the SELECT
  //! clause without aggregations
  bool scalar_executor;
  //! The data chunk of the current physical operator, used to resolve e.g.
  //! column references
  DataChunk* chunk;

  //! The parent executor of this one, if any. Used for subquery evaluation.
  ExpressionExecutor* parent;

  //! The operator state of the current physical operator, used to resolve
  //! e.g. group-by HT lookups
  PhysicalOperatorState* state;

  //! Intermediate vector that is the result of the previously visited
  //! expression
  Vector vector;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP