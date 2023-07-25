#ifndef SUMMERDB_PLANNER_BINDER_HPP
#define SUMMERDB_PLANNER_BINDER_HPP

#include <string>
#include <vector>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Parser/Statement/SqlStatement.hpp"
#include "SummerDB/Planner/BindContext.hpp"

namespace SummerDB {

//! Bind the parsed query tree to the actual columns present in the catalog.
/*!
  The binder is responsible for binding tables and columns to actual physical
  tables and columns in the catalog. In the process, it also resolves types of
  all expressions.
*/
class Binder : public SQLNodeVisitor {
 public:
  Binder(Catalog& catalog) : catalog(catalog) {}

  void Visit(SelectStatement& statement);

  void Visit(BaseTableRefExpression& expr);
  void Visit(ColumnRefExpression& expr);
  void Visit(JoinExpression& expr);
  void Visit(SubqueryExpression& expr);

  //! The BindContext created and used by the Binder.
  std::unique_ptr<BindContext> context;

 private:
  Catalog& catalog;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_BINDER_HPP