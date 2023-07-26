#ifndef SUMMERDB_PARSER_CONSTRAINT_NOT_NULL_CONSTRAINT_HPP
#define SUMMERDB_PARSER_CONSTRAINT_NOT_NULL_CONSTRAINT_HPP

#include "SummerDB/Parser/Constraint.hpp"

namespace SummerDB {

class NotNullConstraint : public Constraint {
 public:
  NotNullConstraint(size_t index)
      : Constraint(ConstraintType::NOT_NULL), index(index){};
  virtual ~NotNullConstraint() {}

  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  virtual std::string ToString() const { return "NOT NULL Constraint"; }

  //! Serialize to a stand-alone binary blob
  virtual void Serialize(Serializer& serializer);
  //! Deserializes a NotNullConstraint
  static std::unique_ptr<Constraint> Deserialize(Deserializer& source);

  //! Column index this constraint pertains to
  size_t index;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_CONSTRAINT_NOT_NULL_CONSTRAINT_HPP