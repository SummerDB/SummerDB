#ifndef SUMMERDB_PARSER_PARSER_HPP
#define SUMMERDB_PARSER_PARSER_HPP

#include <string>
#include <vector>

#include "SummerDB/Parser/Statement/SqlStatement.hpp"

struct Node;
struct List;

namespace SummerDB {

//! The parser is responsible for parsing the query and converting it into a set
//! of parsed statements. The parsed statements can then be converted into a
//! plan and executed.
class Parser {
 public:
  Parser() = default;

  //! Attempts to parse a query into a series of SQL statements. Returns
  //! whether or not the parsing was successful. If the parsing was
  //! successful, the parsed statements will be stored in the statements
  //! variable.
  bool ParseQuery(const char* query);

  //! Returns whether or not the parsing was successful.
  bool GetSuccess() const { return success; }
  //! If the parsing was unsuccessful, returns the error message that was
  //! generated.
  const std::string& GetErrorMessage() const { return message; }

  bool success;
  std::string message;

  //! The parsed SQL statements from an invocation to ParseQuery.
  std::vector<std::unique_ptr<SQLStatement>> statements;

 private:
  //! Transform a Postgres parse tree into a set of SQL Statements
  bool TransformList(List* tree);
  //! Transform a single Postgres parse node into a SQL Statement.
  std::unique_ptr<SQLStatement> TransformNode(Node* stmt);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_PARSER_HPP