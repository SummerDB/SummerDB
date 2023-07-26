#ifndef SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"

namespace SummerDB {

class InsertStatement : public SQLStatement {
 public:
  InsertStatement() : SQLStatement(StatementType::INSERT){};
  virtual ~InsertStatement() {}
  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  //! The select statement to insert from
  std::unique_ptr<SelectStatement> select_statement;

  //! List of values to insert
  std::vector<std::vector<std::unique_ptr<Expression>>> values;

  //! Column names to insert into
  std::vector<std::string> columns;

  //! Table name to insert to
  std::string table;
  //! Schema name to insert to
  std::string schema;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP