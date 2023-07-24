#ifndef SUMMERDB_PARSER_TRANSFORM_HPP
#define SUMMERDB_PARSER_TRANSFORM_HPP

#include "SummerDB/Parser/Postgres/ParseNodes.h"
#include "SummerDB/Parser/Postgres/pg_list.h"
#include "SummerDB/Parser/Postgres/pg_query.h"
#include "SummerDB/Parser/Postgres/pg_trigger.h"
#include "SummerDB/Parser/Statement/CreateStatement.hpp"
#include "SummerDB/Parser/Statement/InsertStatement.hpp"
#include "SummerDB/Parser/Statement/SelectStatement.hpp"

namespace SummerDB {

//! Transform a Postgres T_SelectStmt node into a SelectStatement
std::unique_ptr<SelectStatement> TransformSelect(Node* node);
//! Transform a Postgres T_CreateStmt node into a CreateStatement
std::unique_ptr<CreateStatement> TransformCreate(Node* node);
//! Transform a Postgres T_InsertStmt node into a InsertStatement
std::unique_ptr<InsertStatement> TransformInsert(Node* node);

//! Transform a Postgres column reference into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformColumnRef(ColumnRef* root);
//! Transform a Postgres constant value into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformValue(value val);
//! Transform a Postgres operator into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformAExpr(A_Expr* root);
//! Transform a Postgres abstract expression into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformExpression(Node* node);
//! Transform a Postgres function call into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformFuncCall(FuncCall* root);
//! Transform a Postgres FROM clause into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformFrom(List* root);
//! Transform a Postgres constant value into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformConstant(A_Const* c);
//! Transform a Postgres table reference into an AbstractExpression
std::unique_ptr<AbstractExpression> TransformRangeVar(RangeVar* root);

//! Transform a Postgres TypeName string into a TypeId
TypeId TransformStringToTypeId(char* str);

//! Transform a Postgres GROUP BY expression into a list of AbstractExpression
bool TransformGroupBy(List* group,
                      std::vector<std::unique_ptr<AbstractExpression>>& result);
//! Transform a Postgres ORDER BY expression into an OrderByDescription
bool TransformOrderBy(List* order, OrderByDescription& result);

//! Transform a Postgres SELECT clause into a list of AbstractExpression
bool TransformExpressionList(
    List* list, std::vector<std::unique_ptr<AbstractExpression>>& result);

//! Transform a Postgres Value List into a list of AbstractExpression
bool TransformValueList(
    List* list, std::vector<std::unique_ptr<AbstractExpression>>& result);

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TRANSFORM_HPP