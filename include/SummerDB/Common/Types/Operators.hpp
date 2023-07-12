#ifndef SUMMERDB_COMMON_TYPES_OPERATORS_HPP
#define SUMMERDB_COMMON_TYPES_OPERATORS_HPP

#include <algorithm>
#include <string>

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Types/Date.hpp"
#include "SummerDB/Common/Types/Hash.hpp"

namespace SummerDB {

namespace Operators {

//===--------------------------------------------------------------------===//
// Numeric Operations
//===--------------------------------------------------------------------===//
struct Addition {
  template <class T>
  static inline T Operation(T left, T right) {
    return left + right;
  }
};

struct Subtraction {
  template <class T>
  static inline T Operation(T left, T right) {
    return left - right;
  }
};

struct Multiplication {
  template <class T>
  static inline T Operation(T left, T right) {
    return left * right;
  }
};

struct Division {
  template <class T>
  static inline T Operation(T left, T right) {
    if (right == 0) {
      throw SummerDB::Exception("Division by 0");
    }
    return left / right;
  }
};

struct Modulo {
  template <class T>
  static inline T Operation(T left, T right) {
    return left % right;
  }
};

struct Abs {
  template <class T>
  static inline T Operation(T left) {
    return std::abs(left);
  }
};

struct Not {
  static inline bool Operation(bool left) { return !left; }
};

template <>
double Modulo::Operation(double left, double right);

struct XOR {
  template <class T>
  static inline T Operation(T left, T right) {
    return left ^ right;
  }
};

//===--------------------------------------------------------------------===//
// Comparison Operations
//===--------------------------------------------------------------------===//
struct Equals {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left == right;
  }
};

struct NotEquals {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left != right;
  }
};

struct GreaterThan {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left > right;
  }
};

struct GreaterThanEquals {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left >= right;
  }
};

struct LessThan {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left < right;
  }
};

struct LessThanEquals {
  template <class T>
  static inline bool Operation(T left, T right) {
    return left <= right;
  }
};

struct And {
  static inline bool Operation(bool left, bool right) { return left && right; }
};

struct Or {
  static inline bool Operation(bool left, bool right) { return left || right; }
};

// NULLs are passed in these as well but the result is ignored. So just return 0
struct EqualsVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) == 0 : false;
  }
};

struct NotEqualsVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) != 0 : false;
  }
};

struct LessThanVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) < 0 : false;
  }
};

struct LessThanEqualsVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) <= 0 : false;
  }
};

struct GreaterThanVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) > 0 : false;
  }
};

struct GreaterThanEqualsVarchar {
  static inline bool Operation(char* left, char* right) {
    return (left && right) ? strcmp(left, right) >= 0 : false;
  }
};

//===--------------------------------------------------------------------===//
// Aggregation Operations
//===--------------------------------------------------------------------===//
struct Max {
  template <class T>
  static inline T Operation(T left, T right) {
    return std::max(left, right);
  }
};

struct Min {
  template <class T>
  static inline T Operation(T left, T right) {
    return std::min(left, right);
  }
};

struct PickLeft {
  template <class T>
  static inline T Operation(T left, T right) {
    return left;
  }
};

struct PickRight {
  template <class T>
  static inline T Operation(T left, T right) {
    return right;
  }
};

struct ConstantZero {
  template <class T>
  static inline T Operation(T left, T right) {
    return 0;
  }
};

struct ConstantOne {
  template <class T>
  static inline T Operation(T left, T right) {
    return 1;
  }
};

struct AddOne {
  template <class T>
  static inline T Operation(T left, T right) {
    return right + 1;
  }
};

struct Hash {
  template <class T>
  static inline int32_t Operation(T left, bool is_null) {
    if (is_null)
      return SummerDB::Hash<T>(SummerDB::NullValue<T>());
    return SummerDB::Hash<T>(left);
  }
};

struct AnyTrue {
  static inline uint8_t Operation(uint8_t val, uint8_t result) {
    return result || val == true;
  }
};

struct AllTrue {
  static inline uint8_t Operation(uint8_t val, uint8_t result) {
    return result && val == true;
  }
};

struct MaximumStringLength {
  static inline uint64_t Operation(const char* str, uint64_t right) {
    if (!str) {
      return right;
    }
    return std::max((uint64_t)strlen(str), right);
  }
};

//===--------------------------------------------------------------------===//
// Casts
//===--------------------------------------------------------------------===//
struct Cast {
  template <class SRC, class DST>
  static inline DST Operation(SRC left) {
    return (DST)left;
  }
};

// string casts
// string -> numeric
template <>
int8_t Cast::Operation(const char* left);
template <>
int16_t Cast::Operation(const char* left);
template <>
int Cast::Operation(const char* left);
template <>
int64_t Cast::Operation(const char* left);
template <>
uint64_t Cast::Operation(const char* left);
template <>
double Cast::Operation(const char* left);
// numeric -> string
template <>
const char* Cast::Operation(int8_t left);
template <>
const char* Cast::Operation(int16_t left);
template <>
const char* Cast::Operation(int left);
template <>
const char* Cast::Operation(int64_t left);
template <>
const char* Cast::Operation(uint64_t left);
template <>
const char* Cast::Operation(double left);
// numeric -> std::string
template <>
std::string Cast::Operation(int8_t left);
template <>
std::string Cast::Operation(int16_t left);
template <>
std::string Cast::Operation(int left);
template <>
std::string Cast::Operation(int64_t left);
template <>
std::string Cast::Operation(uint64_t left);
template <>
std::string Cast::Operation(double left);

struct CastFromDate {
  template <class SRC, class DST>
  static inline DST Operation(SRC left) {
    throw SummerDB::NotImplementedException(
        "Cast from date could not be performed!");
  }
};
struct CastToDate {
  template <class SRC, class DST>
  static inline DST Operation(SRC left) {
    throw SummerDB::NotImplementedException(
        "Cast to date could not be performed!");
  }
};

template <>
const char* CastFromDate::Operation(SummerDB::date_t left);
template <>
SummerDB::date_t CastToDate::Operation(const char* left);

struct NOP {
  template <class T>
  static inline T Operation(T left) {
    return left;
  }
};

}  // namespace Operators
}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_TYPES_OPERATORS_HPP