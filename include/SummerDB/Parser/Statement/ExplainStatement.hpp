#ifndef SUMMERDB_PARSER_STATEMENT_EXPLAIN_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_EXPLAIN_STATEMENT_HPP

#include <vector>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class ExplainStatement : public SQLStatement {
 public:
  ExplainStatement(std::unique_ptr<SQLStatement> stmt)
      : SQLStatement(StatementType::EXPLAIN), stmt(std::move(stmt)){};
  std::unique_ptr<SQLStatement> stmt;

  virtual ~ExplainStatement() {}

  virtual std::string ToString() const { return "Explain"; }
  virtual void Accept(SQLNodeVisitor* v) {}
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_EXPLAIN_STATEMENT_HPP