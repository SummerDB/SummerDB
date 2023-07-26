#ifndef SUMMERDB_PARSER_CONSTRAINT_PARSED_CONSTRAINT_HPP
#define SUMMERDB_PARSER_CONSTRAINT_PARSED_CONSTRAINT_HPP

#include <vector>
#include "SummerDB/Parser/Constraint.hpp"

namespace SummerDB {

//! The ParsedConstraint represents either a UNIQUE or PRIMARY KEY constraint
//! that is not fully resolved yet. When added to a table in the catalog, it
//! will be transformed to a proper PRIMARY KEY or UNIQUE constraint
class ParsedConstraint : public Constraint {
 public:
  ParsedConstraint(ConstraintType type, size_t index)
      : Constraint(ConstraintType::DUMMY), ctype(type), index(index) {}
  ParsedConstraint(ConstraintType type, std::vector<std::string> columns)
      : Constraint(ConstraintType::DUMMY),
        ctype(type),
        index((size_t)-1),
        columns(columns) {}
  virtual ~ParsedConstraint() {}

  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  virtual std::string ToString() const { return "Dummy Constraint"; }

  //! Serialize to a stand-alone binary blob
  virtual void Serialize(Serializer& serializer);
  //! Deserializes a ParsedConstraint
  static std::unique_ptr<Constraint> Deserialize(Deserializer& source);

  ConstraintType ctype;
  size_t index;
  std::vector<std::string> columns;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_CONSTRAINT_PARSED_CONSTRAINT_HPP