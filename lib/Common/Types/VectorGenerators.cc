#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/Types/Hash.hpp"
#include "SummerDB/Common/Types/Operators.hpp"
#include "SummerDB/Common/Types/VectorOperations.hpp"

namespace SummerDB {

template <class T>
void _templated_generate_data(Vector& result, T start, T increment) {
  T* ldata = (T*)result.data;
  T value = start;
  for (size_t i = 0; i < result.count; i++) {
    ldata[i] = value;
    value += increment;
  }
}

//===--------------------------------------------------------------------===//
// Copy data from vector
//===--------------------------------------------------------------------===//
void VectorOperations::GenerateSequence(Vector& result, int64_t start,
                                        int64_t increment) {
  if (!TypeIsNumeric(result.type)) {
    throw InvalidTypeException(
        result.type, "Can only generate sequences for numeric values!");
  }

  switch (result.type) {
    case TypeId::TINYINT:
      _templated_generate_data<int8_t>(result, start, increment);
      break;
    case TypeId::SMALLINT:
      _templated_generate_data<int16_t>(result, start, increment);
      break;
    case TypeId::INTEGER:
      _templated_generate_data<int32_t>(result, start, increment);
      break;
    case TypeId::BIGINT:
      _templated_generate_data<int64_t>(result, start, increment);
      break;
    case TypeId::DECIMAL:
      _templated_generate_data<double>(result, start, increment);
      break;
    case TypeId::POINTER:
      _templated_generate_data<uint64_t>(result, start, increment);
      break;
    case TypeId::DATE:
      _templated_generate_data<date_t>(result, start, increment);
      break;
    default:
      throw NotImplementedException("Unimplemented type for generate sequence");
  }
}

}  // namespace SummerDB