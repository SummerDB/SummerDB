#include "SummerDB/Parser/Constraint.hpp"

#include "SummerDB/Common/Serializer.hpp"
#include "SummerDB/Parser/Constraint/HeaderList.hpp"

namespace SummerDB {

void Constraint::Serialize(Serializer& serializer) {
  serializer.Write<int>((int)type);
}

std::unique_ptr<Constraint> Constraint::Deserialize(Deserializer& source) {
  bool failed = false;
  auto type = (ConstraintType)source.Read<int>(failed);
  if (failed) {
    return nullptr;
  }
  switch (type) {
    case ConstraintType::NOT_NULL:
      return NotNullConstraint::Deserialize(source);
    case ConstraintType::CHECK:
      return CheckConstraint::Deserialize(source);
    case ConstraintType::DUMMY:
      return ParsedConstraint::Deserialize(source);
    default:
      // don't know how to serialize this constraint type
      return nullptr;
  }
}

}  // namespace SummerDB