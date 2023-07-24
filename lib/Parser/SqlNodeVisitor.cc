#include "SummerDB/Parser/SqlNodeVisitor.hpp"

#include "SummerDB/Parser/Expression/ExpressionList.hpp"

namespace SummerDB {

void SQLNodeVisitor::Visit(AggregateExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(BaseTableRefExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(TableRefExpression& expr) {
  expr.AcceptChildren(this);
}

}  // namespace SummerDB