#ifndef SUMMERDB_PARSER_TABLEREF_TABLE_REF_HPP
#define SUMMERDB_PARSER_TABLEREF_TABLE_REF_HPP

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

//! Represents a generic expression that returns a table.
class TableRef : public Printable {
 public:
  TableRef(TableReferenceType ref_type) : ref_type(ref_type) {}

  virtual void Accept(SQLNodeVisitor* v) = 0;

  //! Convert the object to a string
  virtual std::string ToString() const { return std::string(); }

  TableReferenceType ref_type;
  std::string alias;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TABLEREF_TABLE_REF_HPP