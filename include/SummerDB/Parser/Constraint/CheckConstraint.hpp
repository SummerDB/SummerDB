#ifndef SUMMERDB_PARSER_CONSTRAINT_CHECK_CONSTRAINT_HPP
#define SUMMERDB_PARSER_CONSTRAINT_CHECK_CONSTRAINT_HPP

#include "SummerDB/Common/StringUtil.hpp"
#include "SummerDB/Parser/Constraint.hpp"
#include "SummerDB/Parser/Expression.hpp"

namespace SummerDB {

//! The CheckConstraint contains an expression that must evaluate to TRUE for
//! every row in a table
class CheckConstraint : public Constraint {
 public:
  CheckConstraint(std::unique_ptr<Expression> expression)
      : Constraint(ConstraintType::CHECK), expression(std::move(expression)){};
  virtual ~CheckConstraint() {}

  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  virtual std::string ToString() const {
    return StringUtil::Format("CHECK(%s)", expression->ToString().c_str());
  }

  //! Serialize to a stand-alone binary blob
  virtual void Serialize(Serializer& serializer);
  //! Deserializes a CheckConstraint
  static std::unique_ptr<Constraint> Deserialize(Deserializer& source);

  std::unique_ptr<Expression> expression;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_CONSTRAINT_CHECK_CONSTRAINT_HPP