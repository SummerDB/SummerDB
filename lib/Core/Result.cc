#include "SummerDB/Core/Result.hpp"

namespace SummerDB {

SummerDBResult::SummerDBResult() : success(true) {}

SummerDBResult::SummerDBResult(std::string error)
    : success(false), error(error) {}

void SummerDBResult::Print() {
  if (success) {
    for (auto& type : types()) {
      printf("%s\t", TypeIdToString(type).c_str());
    }
    printf(" [ %zu ]\n", size());
    for (size_t j = 0; j < size(); j++) {
      for (size_t i = 0; i < column_count(); i++) {
        printf("%s\t", collection.GetValue(i, j).ToString().c_str());
      }
      printf("\n");
    }
    printf("\n");
  } else {
    fprintf(stderr, "Query Error: %s\n", error.c_str());
  }
}

}  // namespace SummerDB