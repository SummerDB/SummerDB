#ifndef SUMMERDB_PLANNER_LOGICAL_OPERATOR_VISITOR_HPP
#define SUMMERDB_PLANNER_LOGICAL_OPERATOR_VISITOR_HPP

#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

class LogicalAggregate;
class LogicalCreate;
class LogicalCrossProduct;
class LogicalDelete;
class LogicalDistinct;
class LogicalFilter;
class LogicalGet;
class LogicalJoin;
class LogicalLimit;
class LogicalOperator;
class LogicalOrder;
class LogicalProjection;
class LogicalInsert;
class LogicalCopy;
class LogicalExplain;
class LogicalUnion;
class LogicalUpdate;

//! The LogicalOperatorVisitor is an abstract base class that implements the
//! Visitor pattern on LogicalOperator.
class LogicalOperatorVisitor : public SQLNodeVisitor {
 public:
  virtual ~LogicalOperatorVisitor(){};

  virtual void VisitOperator(LogicalOperator& op);
  using SQLNodeVisitor::Visit;

  virtual void Visit(LogicalAggregate& op);
  virtual void Visit(LogicalCreate& op);
  virtual void Visit(LogicalCrossProduct& op);
  virtual void Visit(LogicalDelete& op);
  virtual void Visit(LogicalFilter& op);
  virtual void Visit(LogicalGet& op);
  virtual void Visit(LogicalJoin& op);
  virtual void Visit(LogicalLimit& op);
  virtual void Visit(LogicalOrder& op);
  virtual void Visit(LogicalProjection& op);
  virtual void Visit(LogicalInsert& op);
  virtual void Visit(LogicalCopy& op);
  virtual void Visit(LogicalExplain& op);
  virtual void Visit(LogicalUnion& op);
  virtual void Visit(LogicalUpdate& op);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_LOGICAL_OPERATOR_VISITOR_HPP