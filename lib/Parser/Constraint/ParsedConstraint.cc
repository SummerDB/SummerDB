#include "SummerDB/Parser/Constraint/ParsedConstraint.hpp"

#include "SummerDB/Common/Serializer.hpp"

namespace SummerDB {

void ParsedConstraint::Serialize(Serializer& serializer) {
  Constraint::Serialize(serializer);
  serializer.Write<int>((int)ctype);
  serializer.Write<size_t>(index);
  serializer.Write<uint32_t>(columns.size());
  for (auto& column : columns) {
    serializer.WriteString(column);
  }
}

std::unique_ptr<Constraint> ParsedConstraint::Deserialize(
    Deserializer& source) {
  bool failed = false;
  auto type = (ConstraintType)source.Read<int>(failed);
  auto index = source.Read<size_t>(failed);
  auto column_count = source.Read<uint32_t>(failed);
  if (failed) {
    return nullptr;
  }
  if (index != (size_t)-1) {
    // single column parsed constraint
    return make_unique_base<Constraint, ParsedConstraint>(type, index);
  } else {
    // column list parsed constraint
    std::vector<std::string> columns;
    for (size_t i = 0; i < column_count; i++) {
      auto column_name = source.Read<std::string>(failed);
      if (failed) {
        return nullptr;
      }
      columns.push_back(column_name);
    }
    return make_unique_base<Constraint, ParsedConstraint>(type, columns);
  }
}

}  // namespace SummerDB