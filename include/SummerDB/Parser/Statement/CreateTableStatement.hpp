#ifndef SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP

#include <vector>

#include "SummerDB/Catalog/ColumnDefinition.hpp"
#include "SummerDB/Parser/ParsedData.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class CreateTableStatement : public SQLStatement {
 public:
  CreateTableStatement()
      : SQLStatement(StatementType::CREATE_TABLE),
        info(make_unique<CreateTableInformation>()){};
  virtual ~CreateTableStatement() {}

  virtual std::string ToString() const { return "CREATE TABLE"; }
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  std::unique_ptr<CreateTableInformation> info;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_CREATE_STATEMENT_HPP