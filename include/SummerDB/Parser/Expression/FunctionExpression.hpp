#ifndef SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"

namespace SummerDB {

//! Represents a function call
class FunctionExpression : public AbstractExpression {
 public:
  FunctionExpression(const char* func_name,
                     std::vector<std::unique_ptr<AbstractExpression>>& children)
      : AbstractExpression(ExpressionType::FUNCTION), func_name(func_name) {
    for (auto& child : children) {
      AddChild(std::move(child));
    }
  }

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }

 private:
  std::string func_name;
  std::vector<TypeId> func_arg_types;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_FUNCTION_EXPRESSION_HPP