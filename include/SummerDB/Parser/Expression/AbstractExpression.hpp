#ifndef SUMMERDB_PARSER_EXPRESSION_ABSTRACT_EXPRESSION_HPP
#define SUMMERDB_PARSER_EXPRESSION_ABSTRACT_EXPRESSION_HPP

#include <memory>
#include <vector>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

class AggregateExpression;

//!  AbstractExpression class is a base class that can represent any expression
//!  part of a SQL statement.
/*!
  The AbstractExpression class is a base class that can represent any expression
  part of a SQL statement. This is, for example, a column reference in a SELECT
  clause, but also operators, aggregates or filters.

  In the execution engine, an AbstractExpression always returns a single Vector
  of the type specified by return_type. It can take an arbitrary amount of
  Vectors as input (but in most cases the amount of input vectors is 0-2).
*/
class AbstractExpression : public Printable {
 public:
  //! Create an AbstractExpression
  AbstractExpression(ExpressionType type) : type(type), parent(nullptr) {}
  //! Create an AbstractExpression with zero, one or two children with the
  //! specified return type
  AbstractExpression(ExpressionType type, TypeId return_type,
                     std::unique_ptr<AbstractExpression> left = nullptr,
                     std::unique_ptr<AbstractExpression> right = nullptr)
      : type(type), return_type(return_type), parent(nullptr) {
    if (left)
      AddChild(std::move(left));
    if (right)
      AddChild(std::move(right));
  }

  virtual void Accept(SQLNodeVisitor*) = 0;
  virtual void AcceptChildren(SQLNodeVisitor* v) {
    for (auto& child : children) {
      child->Accept(v);
    }
  }

  //! Resolves the type for this expression based on its children
  virtual void ResolveType() {
    for (auto& child : children) {
      child->ResolveType();
    }
  }

  //! Add a child node to the AbstractExpression. Note that the order of
  //! adding children is important in most cases
  void AddChild(std::unique_ptr<AbstractExpression> child) {
    child->parent = this;
    children.push_back(std::move(child));
  }

  //! Return a list of the deepest aggregates that are present in the
  //! AbstractExpression (if any).
  /*!
    This function is used by the execution engine to figure out which
    aggregates/groupings have to be computed.

    Examples:

    (1) SELECT SUM(a) + SUM(b) FROM table; (Two aggregates, SUM(a) and SUM(b))

    (2) SELECT COUNT(SUM(a)) FROM table; (One aggregate, SUM(a))
  */
  virtual void GetAggregates(std::vector<AggregateExpression*>& expressions);
  //! Returns true if this AbstractExpression is an aggregate or not.
  /*!
    Examples:

    (1) SUM(a) + 1 -- True

    (2) a + 1 -- False
  */
  virtual bool IsAggregate();

  //! Returns the type of the expression
  ExpressionType GetExpressionType() { return type; }

  //! Type of the expression
  ExpressionType type;
  //! Return type of the expression. This must be known in the execution
  //! engine
  TypeId return_type = TypeId::INVALID;

  //! The alias of the expression, used in the SELECT clause (e.g. SELECT x +
  //! 1 AS f)
  std::string alias;

  //! The parent node of the expression, if any
  AbstractExpression* parent;

  //! A list of children of the expression
  std::vector<std::unique_ptr<AbstractExpression>> children;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_EXPRESSION_ABSTRACT_EXPRESSION_HPP