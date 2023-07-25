#ifndef SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP
#define SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP

#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Common/Types/DataChunk.hpp"
#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

//! ExpressionExecutor is responsible for executing an arbitrary
//! AbstractExpression and returning a Vector
/*!
    ExpressionExecutor is responsible for executing an arbitrary
   AbstractExpression and returning a Vector. It executes the expressions
   recursively using a visitor pattern.
*/
class ExpressionExecutor : public SQLNodeVisitor {
 public:
  ExpressionExecutor(DataChunk& chunk, PhysicalOperatorState* state = nullptr)
      : chunk(chunk), state(state) {}

  void Reset();

  //! Execute a single abstract expression and store the result in result
  void Execute(AbstractExpression* expr, Vector& result);
  //! Execute the abstract expression, and "logical AND" the result together
  //! with result
  void Merge(AbstractExpression* expr, Vector& result);
  //! Execute the given aggregate expression, and merge the result together
  //! with v
  void Merge(AggregateExpression& expr, Value& v);

  void Visit(AggregateExpression& expr);
  void Visit(BaseTableRefExpression& expr);
  void Visit(CastExpression& expr);
  void Visit(ColumnRefExpression& expr);
  void Visit(ComparisonExpression& expr);
  void Visit(ConjunctionExpression& expr);
  void Visit(ConstantExpression& expr);
  void Visit(CrossProductExpression& expr);
  void Visit(FunctionExpression& expr);
  void Visit(GroupRefExpression& expr);
  void Visit(JoinExpression& expr);
  void Visit(OperatorExpression& expr);
  void Visit(SubqueryExpression& expr);
  void Visit(TableRefExpression& expr);

 private:
  //! The data chunk of the current physical operator, used to resolve e.g.
  //! column references
  DataChunk& chunk;
  //! The operator state of the current physical operator, used to resolve
  //! e.g. group-by HT lookups
  PhysicalOperatorState* state;

  //! Intermediate vector that is the result of the previously visited
  //! expression
  Vector vector;
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_EXPRESSION_EXECUTOR_HPP