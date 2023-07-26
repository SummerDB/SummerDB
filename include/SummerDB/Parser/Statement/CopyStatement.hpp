#ifndef SUMMERDB_PARSER_STATEMENT_COPY_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_COPY_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class CopyStatement : public SQLStatement {
 public:
  CopyStatement() : SQLStatement(StatementType::COPY){};
  virtual ~CopyStatement() {}
  virtual std::string ToString() const;
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  std::string table;
  std::string schema;

  // The SQL statement used instead of a table when copying data out to a file
  std::unique_ptr<SQLStatement> select_statement;

  std::string file_path;

  // List of Columns that will be copied from/to.
  std::vector<std::string> select_list;

  // File Format
  ExternalFileFormat format = ExternalFileFormat::CSV;

  // Copy: From CSV (True) To CSV (False)
  bool is_from;

  char delimiter = ',';
  char quote = '"';
  char escape = '"';
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_COPY_STATEMENT_HPP