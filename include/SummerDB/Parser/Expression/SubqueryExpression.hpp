#ifndef SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP

#include "SummerDB/Execution/PhysicalOperator.hpp"
#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"
#include "SummerDB/Parser/Tableref/TableRef.hpp"
#include "SummerDB/Planner/BindContext.hpp"
#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

enum class SubqueryType { INVALID = 0, DEFAULT = 1, EXISTS = 2, IN = 3 };

//! Represents a subquery
class SubqueryExpression : public Expression {
 public:
  SubqueryExpression()
      : Expression(ExpressionType::SELECT_SUBQUERY),
        type(SubqueryType::DEFAULT) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  std::unique_ptr<SelectStatement> subquery;
  std::unique_ptr<LogicalOperator> op;
  std::unique_ptr<BindContext> context;
  std::unique_ptr<PhysicalOperator> plan;
  SubqueryType type;
  bool is_correlated = false;

  virtual std::string ToString() const override {
    std::string result = GetExprName();
    if (op) {
      result += "(" + op->ToString() + ")";
    }
    return result;
  }

  virtual bool HasSubquery() override { return true; }

  virtual bool IsScalar() override { return false; }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_SUBQUERY_EXPRESSION_HPP