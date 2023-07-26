#ifndef SUMMERDB_PARSER_STATEMENT_UPDATE_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_UPDATE_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class UpdateStatement : public SQLStatement {
 public:
  UpdateStatement() : SQLStatement(StatementType::UPDATE) {}
  virtual ~UpdateStatement() {}
  virtual std::string ToString() const { return "Update"; }
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  std::unique_ptr<Expression> condition;
  std::unique_ptr<TableRef> table;

  std::vector<std::string> columns;
  std::vector<std::unique_ptr<Expression>> expressions;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_UPDATE_STATEMENT_HPP