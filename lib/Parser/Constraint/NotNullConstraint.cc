#include "SummerDB/Parser/Constraint/NotNullConstraint.hpp"

#include "SummerDB/Common/Serializer.hpp"

namespace SummerDB {

void NotNullConstraint::Serialize(Serializer& serializer) {
  Constraint::Serialize(serializer);
  serializer.Write<size_t>(index);
}

std::unique_ptr<Constraint> NotNullConstraint::Deserialize(
    Deserializer& source) {
  bool failed = false;
  auto index = source.Read<size_t>(failed);
  if (failed) {
    return nullptr;
  }
  return make_unique_base<Constraint, NotNullConstraint>(index);
}

}  // namespace SummerDB