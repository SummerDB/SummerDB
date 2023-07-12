#ifndef SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP

#include <vector>

#include "SummerDB/Catalog/Catalog.hpp"
#include "SummerDB/Catalog/ColumnCatalog.hpp"
#include "SummerDB/Parser/Expression/AbstractExpression.hpp"
#include "SummerDB/Parser/Statement/SqlStatement.hpp"

namespace SummerDB {

class CreateStatement : public SQLStatement {
 public:
  CreateStatement()
      : SQLStatement(StatementType::CREATE_TABLE), schema(DEFAULT_SCHEMA){};
  virtual ~CreateStatement() {}

  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  //! Table name to insert to
  std::string table;
  //! Schema name to insert to
  std::string schema;

  //! List of columns of the table
  std::vector<ColumnCatalogEntry> columns;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP