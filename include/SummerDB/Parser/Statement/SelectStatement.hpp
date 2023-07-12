#ifndef SUMMERDB_PARSER_STATEMENT_SELECT_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_SELECT_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/Statement/SqlStatement.hpp"

namespace SummerDB {

//! GROUP BY description
struct GroupByDescription {
  //! List of groups
  std::vector<std::unique_ptr<AbstractExpression>> groups;
  //! HAVING clause
  std::unique_ptr<AbstractExpression> having;
};
//! Single node in ORDER BY statement
struct OrderByNode {
  //! Sort order, ASC or DESC
  OrderType type;
  //! Expression to order by
  std::unique_ptr<AbstractExpression> expression;

  OrderByNode() {}
  OrderByNode(OrderType type, std::unique_ptr<AbstractExpression> expression)
      : type(type), expression(std::move(expression)) {}
};
//! ORDER BY description
struct OrderByDescription {
  //! List of order nodes
  std::vector<OrderByNode> orders;
};
//! LIMIT description
struct LimitDescription {
  //! LIMIT count
  int64_t limit = -1;
  //! OFFSET
  int64_t offset = -1;
};

//! SelectStatement is a typical SELECT clause
class SelectStatement : public SQLStatement {
 public:
  SelectStatement()
      : SQLStatement(StatementType::SELECT),
        union_select(nullptr),
        select_distinct(false){};
  virtual ~SelectStatement() {}

  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  //! The projection list
  std::vector<std::unique_ptr<AbstractExpression>> select_list;
  //! The FROM clause
  std::unique_ptr<AbstractExpression> from_table;
  //! The WHERE clause
  std::unique_ptr<AbstractExpression> where_clause;
  //! DISTINCT or not
  bool select_distinct;

  //! Group By Description
  GroupByDescription groupby;
  //! Order By Description
  OrderByDescription orderby;
  //! Limit Description
  LimitDescription limit;

  //! Whether or not the query has a LIMIT clause
  bool HasLimit() { return limit.limit >= 0; }
  //! Whether or not the query has a GROUP BY clause
  bool HasGroup() { return groupby.groups.size() > 0; }
  //! Whether or not the query has a HAVING clause
  bool HasHaving() { return groupby.having.get(); }
  //! Whether or not the query has an ORDER BY clause
  bool HasOrder() { return orderby.orders.size() > 0; }
  //! Whether or not the query has an AGGREGATION
  bool HasAggregation();

  std::unique_ptr<SelectStatement> union_select;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_SELECT_STATEMENT_HPP