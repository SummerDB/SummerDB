#ifndef SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/Expression/BaseTableRefExpression.hpp"
#include "SummerDB/Parser/Statement/SqlStatement.hpp"

namespace SummerDB {

class InsertStatement : public SQLStatement {
 public:
  InsertStatement() : SQLStatement(StatementType::INSERT){};
  virtual ~InsertStatement() {}
  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  //! List of values to insert
  std::vector<std::unique_ptr<AbstractExpression>> values;

  //! Table name to insert to
  std::string table;
  //! Schema name to insert to
  std::string schema;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_INSERT_STATEMENT_HPP