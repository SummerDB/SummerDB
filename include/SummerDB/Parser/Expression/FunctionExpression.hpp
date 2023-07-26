#ifndef SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP

#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! Represents a function call
class FunctionExpression : public Expression {
 public:
  FunctionExpression(const char* func_name,
                     std::vector<std::unique_ptr<Expression>>& children)
      : Expression(ExpressionType::FUNCTION),
        func_name(StringUtil::Lower(func_name)) {
    for (auto& child : children) {
      AddChild(std::move(child));
    }
  }

  virtual void ResolveType() override {
    Expression::ResolveType();
    if (func_name == "abs") {
      return_type = children[0]->return_type;
    }
  }

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  std::string func_name;

 private:
  std::vector<TypeId> func_arg_types;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP