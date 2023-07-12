#ifndef SUMMERDB_PARSER_STATEMENT_SQL_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_SQL_STATEMENT_HPP

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

class SelectStatement;

//! SQLStatement is the base class of any type of SQL statement.
class SQLStatement : public Printable {
 public:
  SQLStatement(StatementType type) : stmt_type(type){};
  virtual ~SQLStatement() {}

  StatementType GetType() const { return stmt_type; }

  virtual void Accept(SQLNodeVisitor*) = 0;

 private:
  StatementType stmt_type;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_SQL_STATEMENT_HPP