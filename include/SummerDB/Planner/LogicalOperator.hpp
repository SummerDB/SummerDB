#ifndef SUMMERDB_PLANNER_LOGICAL_OPERATOR_HPP
#define SUMMERDB_PLANNER_LOGICAL_OPERATOR_HPP

#include <vector>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"
#include "SummerDB/Planner/LogicalVisitor.hpp"

namespace SummerDB {

//! LogicalOperator is the base class of the logical operators present in the
//! logical query tree
class LogicalOperator : public Printable {
 public:
  LogicalOperator(LogicalOperatorType type) : type(type) {}

  LogicalOperatorType GetOperatorType() { return type; }

  virtual std::string ToString() const override {
    std::string result = LogicalOperatorToString(type);
    if (children.size() > 0) {
      result += " ( ";
      for (auto& child : children) {
        result += child->ToString();
      }
      result += " )";
    }
    return result;
  }

  virtual void Accept(LogicalOperatorVisitor*) = 0;
  virtual void AcceptChildren(LogicalOperatorVisitor* v) {
    for (auto& child : children) {
      child->Accept(v);
    }
  }

  //! The type of the logical operator
  LogicalOperatorType type;
  //! The set of children of the operator
  std::vector<std::unique_ptr<LogicalOperator>> children;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_LOGICAL_OPERATOR_HPP