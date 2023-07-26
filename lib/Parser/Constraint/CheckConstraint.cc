#include "SummerDB/Parser/Constraint/CheckConstraint.hpp"

#include "SummerDB/Common/Serializer.hpp"

namespace SummerDB {

void CheckConstraint::Serialize(Serializer& serializer) {
  Constraint::Serialize(serializer);
  expression->Serialize(serializer);
}

std::unique_ptr<Constraint> CheckConstraint::Deserialize(Deserializer& source) {
  auto expression = Expression::Deserialize(source);
  if (!expression) {
    return nullptr;
  }
  return make_unique_base<Constraint, CheckConstraint>(std::move(expression));
}

}  // namespace SummerDB