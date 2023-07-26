#include "SummerDB/Parser/SqlNodeVisitor.hpp"

#include "SummerDB/Parser/Constraint/HeaderList.hpp"
#include "SummerDB/Parser/Expression/HeaderList.hpp"
#include "SummerDB/Parser/Tableref/HeaderList.hpp"

namespace SummerDB {

void SQLNodeVisitor::Visit(SelectStatement& statement) {}

void SQLNodeVisitor::Visit(AggregateExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(CaseExpression& expr) { expr.AcceptChildren(this); }

void SQLNodeVisitor::Visit(CastExpression& expr) { expr.AcceptChildren(this); }

void SQLNodeVisitor::Visit(ColumnRefExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(ComparisonExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(ConjunctionExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(ConstantExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(DefaultExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(FunctionExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(GroupRefExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(OperatorExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(SubqueryExpression& expr) {
  expr.AcceptChildren(this);
}

void SQLNodeVisitor::Visit(CheckConstraint& expr) {
  expr.expression->Accept(this);
}

void SQLNodeVisitor::Visit(NotNullConstraint& expr) {}

void SQLNodeVisitor::Visit(ParsedConstraint& expr) {}

void SQLNodeVisitor::Visit(BaseTableRef& expr) {}

void SQLNodeVisitor::Visit(CrossProductRef& expr) {
  expr.left->Accept(this);
  expr.right->Accept(this);
}

void SQLNodeVisitor::Visit(JoinRef& expr) {
  expr.left->Accept(this);
  expr.right->Accept(this);
}

void SQLNodeVisitor::Visit(SubqueryRef& expr) {}

}  // namespace SummerDB