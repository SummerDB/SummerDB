#ifndef SUMMERDB_PARSER_EXPRESSION_CASE_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_CASE_EXPRESSION_HPP

#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

class CaseExpression : public Expression {
 public:
  // this expression has 3 children, the test and the result if the test
  // evaluates to 1 and the result if it does not
  CaseExpression() : Expression(ExpressionType::OPERATOR_CASE_EXPR) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  virtual void ResolveType() override {
    Expression::ResolveType();
    return_type = std::max(children[1]->return_type, children[2]->return_type);
  }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_CASE_EXPRESSION_HPP