#ifndef SUMMERDB_PARSER_TABLEREF_BASE_TABLE_REF_HPP
#define SUMMERDB_PARSER_TABLEREF_BASE_TABLE_REF_HPP

#include "SummerDB/Parser/SqlNodeVisitor.hpp"
#include "SummerDB/Parser/Tableref/TableRef.hpp"

namespace SummerDB {

//! Represents a TableReference to a base table in the schema
class BaseTableRef : public TableRef {
 public:
  BaseTableRef() : TableRef(TableReferenceType::BASE_TABLE) {}

  virtual void Accept(SQLNodeVisitor* v) override { v->Visit(*this); }

  //! Database name, not used
  std::string database_name;
  //! Schema name
  std::string schema_name;
  //! Table name
  std::string table_name;

  virtual std::string ToString() const override {
    return "GET(" + schema_name + "." + table_name + ")";
  }
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_TABLEREF_BASE_TABLE_REF_HPP