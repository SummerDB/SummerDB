#ifndef SUMMERDB_PARSER_STATEMENT_DELETE_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_DELETE_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class DeleteStatement : public SQLStatement {
 public:
  DeleteStatement() : SQLStatement(StatementType::DELETE) {}
  virtual ~DeleteStatement() {}
  virtual std::string ToString() const { return "Delete"; }
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  std::unique_ptr<Expression> condition;
  std::unique_ptr<TableRef> table;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_DELETE_STATEMENT_HPP