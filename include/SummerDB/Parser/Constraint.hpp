#ifndef SUMMERDB_PARSER_CONSTRAINT_HPP
#define SUMMERDB_PARSER_CONSTRAINT_HPP

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Parser/SqlNodeVisitor.hpp"

namespace SummerDB {

//! Constraint is the base class of any type of table constraint.
class Constraint : public Printable {
 public:
  Constraint(ConstraintType type) : type(type){};
  virtual ~Constraint() {}

  virtual void Accept(SQLNodeVisitor*) = 0;

  ConstraintType type;

  //! Serializes a Constraint to a stand-alone binary blob
  virtual void Serialize(Serializer& serializer);
  //! Deserializes a blob back into a Constraint, returns NULL if
  //! deserialization is not possible
  static std::unique_ptr<Constraint> Deserialize(Deserializer& source);
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_CONSTRAINT_HPP