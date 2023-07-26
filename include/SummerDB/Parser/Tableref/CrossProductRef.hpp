#ifndef SUMMERDB_PARSER_TABLEREF_CROSS_PRODUCT_REF_HPP
#define SUMMERDB_PARSER_TABLEREF_CROSS_PRODUCT_REF_HPP

#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Parser/Tableref/TableRef.hpp"

namespace SummerDB {

//! Represents a cross product
class CrossProductRef : public TableRef {
 public:
  CrossProductRef() : TableRef(TableReferenceType::CROSS_PRODUCT) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  //! The left hand side of the cross product
  std::unique_ptr<TableRef> left;
  //! The right hand side of the cross product
  std::unique_ptr<TableRef> right;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TABLEREF_CROSS_PRODUCT_REF_HPP