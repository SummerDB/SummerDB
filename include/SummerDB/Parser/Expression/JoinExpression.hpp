#ifndef SUMMERDB_PARSER_EXPRESSION_JOIN_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_JOIN_EXPRESSION_HPP

#include "SummerDB/Parser/Expression/TableRefExpression.hpp"

namespace SummerDB {

//! Represents a JOIN between two expressions
class JoinExpression : public TableRefExpression {
 public:
  JoinExpression() : TableRefExpression(TableReferenceType::JOIN) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }
  virtual std::string ToString() const override { return std::string(); }

  //! The left hand side of the join
  std::unique_ptr<AbstractExpression> left;
  //! The right hand side of the join
  std::unique_ptr<AbstractExpression> right;
  //! The join condition
  std::unique_ptr<AbstractExpression> condition;
  //! The join type
  JoinType type;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_JOIN_EXPRESSION_HPP