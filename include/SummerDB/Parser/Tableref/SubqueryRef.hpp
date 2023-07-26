#ifndef SUMMERDB_PARSER_TABLEREF_SUBQUERY_REF_HPP
#define SUMMERDB_PARSER_TABLEREF_SUBQUERY_REF_HPP

#include "SummerDB/Parser/Statement/SelectStatement.hpp"
#include "SummerDB/Parser/Tableref/TableRef.hpp"

namespace SummerDB {

//! Represents a subquery
class SubqueryRef : public TableRef {
 public:
  SubqueryRef() : TableRef(TableReferenceType::SUBQUERY) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  std::unique_ptr<SelectStatement> subquery;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TABLEREF_SUBQUERY_REF_HPP