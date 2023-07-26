#ifndef SUMMERDB_PARSER_STATEMENT_DROP_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_DROP_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class DropStatement : public SQLStatement {
 public:
  DropStatement() : SQLStatement(StatementType::DROP), schema(DEFAULT_SCHEMA){};
  virtual ~DropStatement() {}

  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  //! Table name to drop
  std::string table;
  //! Schema name to drop from
  std::string schema;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_DROP_STATEMENT_HPP