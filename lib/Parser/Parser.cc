#include "SummerDB/Parser/Parser.hpp"

#include <stdio.h>

#include "SummerDB/Parser/Transform.hpp"

namespace SummerDB {

using namespace postgres;

Parser::Parser() : success(false) {}

bool Parser::ParseQuery(const char* query) {
  // first we use the postgres parser to parse the query
  auto context = pg_query_parse_init();
  auto result = pg_query_parse(query);

  // check if it succeeded
  this->success = false;
  if (result.error) {
    this->message = std::string(result.error->message) + "[" +
                    std::to_string(result.error->lineno) + ":" +
                    std::to_string(result.error->cursorpos) + "]";
    goto wrapup;
  }

  try {
    // if it succeeded, we transform the Postgres parse tree into a list of
    // SQLStatements
    if (!TransformList(result.tree)) {
      goto wrapup;
    }
    this->success = true;
  } catch (Exception& ex) {
    this->message = ex.GetMessage();
  } catch (...) {
    this->message = "UNHANDLED EXCEPTION TYPE THROWN IN PARSER!";
  }
wrapup:
  pg_query_parse_finish(context);
  pg_query_free_parse_result(result);
  return this->success;
}

bool Parser::TransformList(List* tree) {
  for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
    auto stmt = TransformNode((Node*)entry->data.ptr_value);
    if (!stmt) {
      statements.clear();
      return false;
    }
    statements.push_back(std::move(stmt));
  }
  return true;
}

std::unique_ptr<SQLStatement> Parser::TransformNode(Node* stmt) {
  switch (stmt->type) {
    case T_SelectStmt:
      return TransformSelect(stmt);
    case T_CreateStmt:
      return TransformCreate(stmt);
    case T_DropStmt:
      return TransformDrop(stmt);
    case T_InsertStmt:
      return TransformInsert(stmt);
    case T_CopyStmt:
      return TransformCopy(stmt);
    case T_TransactionStmt:
      return TransformTransaction(stmt);
    case T_DeleteStmt:
      return TransformDelete(stmt);
    case T_UpdateStmt:
      return TransformUpdate(stmt);
    case T_ExplainStmt: {
      ExplainStmt* explain_stmt = reinterpret_cast<ExplainStmt*>(stmt);
      return std::make_unique<ExplainStatement>(
          TransformNode(explain_stmt->query));
    }
    default:
      throw NotImplementedException("A_Expr not implemented!");
  }
  return nullptr;
}

}  // namespace SummerDB