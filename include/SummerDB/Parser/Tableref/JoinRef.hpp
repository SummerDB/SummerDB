#ifndef SUMMERDB_PARSER_TABLEREF_JOIN_REF_HPP
#define SUMMERDB_PARSER_TABLEREF_JOIN_REF_HPP

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Parser/Tableref/TableRef.hpp"

namespace SummerDB {

//! Represents a JOIN between two expressions
class JoinRef : public TableRef {
 public:
  JoinRef() : TableRef(TableReferenceType::JOIN) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  //! The left hand side of the join
  std::unique_ptr<TableRef> left;
  //! The right hand side of the join
  std::unique_ptr<TableRef> right;
  //! The join condition
  std::unique_ptr<Expression> condition;
  //! The join type
  JoinType type;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TABLEREF_JOIN_REF_HPP