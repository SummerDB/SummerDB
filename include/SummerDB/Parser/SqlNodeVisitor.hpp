#ifndef SUMMERDB_PARSER_SQL_NODE_VISITOR_HPP
#define SUMMERDB_PARSER_SQL_NODE_VISITOR_HPP

namespace SummerDB {

class SelectStatement;
class CreateStatement;
class InsertStatement;

class AggregateExpression;
class TableRefExpression;
class BaseTableRefExpression;

//! The SQLNodeVisitor is an abstract base class that implements the Visitor
//! pattern on AbstractExpression and SQLStatement. It will visit nodes
//! recursively and call the Visit expression corresponding to the expression
//! visited.
class SQLNodeVisitor {
 public:
  virtual ~SQLNodeVisitor(){};

  virtual void Visit(SelectStatement&) {}
  virtual void Visit(CreateStatement&) {}
  virtual void Visit(InsertStatement&) {}

  virtual void Visit(AggregateExpression& expr);
  virtual void Visit(TableRefExpression& expr);
  virtual void Visit(BaseTableRefExpression& expr);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_SQL_NODE_VISITOR_HPP