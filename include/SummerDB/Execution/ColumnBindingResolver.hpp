#ifndef SUMMERDB_EXECUTION_COLUMN_BINDING_RESOLVER_HPP
#define SUMMERDB_EXECUTION_COLUMN_BINDING_RESOLVER_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"
#include "SummerDB/Planner/LogicalOperatorVisitor.hpp"
#include "SummerDB/SummerDB.hpp"

namespace SummerDB {

struct BoundTable {
  size_t table_index;
  size_t column_count;
  size_t column_offset;
};

//! The ColumnBindingResolver resolves ColumnBindings into base tables
//! (table_index, column_index) into physical indices into the DataChunks that
//! are used within the execution engine
class ColumnBindingResolver : public LogicalOperatorVisitor {
 public:
  ColumnBindingResolver() : current_depth(0) {}
  using LogicalOperatorVisitor::Visit;

  void Visit(LogicalUnion& op);
  void Visit(LogicalCrossProduct& op);
  void Visit(LogicalGet& op);
  void Visit(LogicalJoin& op);

  void Visit(ColumnRefExpression& expr);
  void Visit(SubqueryExpression& expr);

  std::vector<BoundTable> bound_tables;
  size_t current_depth;

 private:
  //! Append a list of tables to the current set of bound tables
  void AppendTables(std::vector<BoundTable>& right_tables);
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_COLUMN_BINDING_RESOLVER_HPP