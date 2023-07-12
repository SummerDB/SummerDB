#include "SummerDB/Common/Types/Operators.hpp"

#include <cstdlib>

#include "SummerDB/Common/Types/Date.hpp"

namespace SummerDB {
namespace Operators {

template <>
double Modulo::Operation(double left, double right) {
  throw NotImplementedException("Modulo for double not implemented!");
}

template <>
int8_t Cast::Operation(const char* left) {
  int64_t value = Cast::Operation<const char*, int64_t>(left);
  if (in_bounds<int8_t>(value))
    return (int8_t)value;
  throw std::out_of_range("Cannot convert to TINYINT");
}

template <>
int16_t Cast::Operation(const char* left) {
  int64_t value = Cast::Operation<const char*, int64_t>(left);
  if (in_bounds<int16_t>(value))
    return (int16_t)value;
  throw std::out_of_range("Cannot convert to SMALLINT");
}

template <>
int Cast::Operation(const char* left) {
  return std::stoi(left, NULL, 10);
}

template <>
int64_t Cast::Operation(const char* left) {
  return std::stoll(left, NULL, 10);
}

template <>
uint64_t Cast::Operation(const char* left) {
  return std::stoull(left, NULL, 10);
}

template <>
double Cast::Operation(const char* left) {
  return std::stod(left, NULL);
}

const char* str_to_cstr(std::string str) {
  char* cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return cstr;
}

// numeric -> string
template <>
std::string Cast::Operation(int8_t left) {
  return std::to_string(left);
}

template <>
std::string Cast::Operation(int16_t left) {
  return std::to_string(left);
}

template <>
std::string Cast::Operation(int left) {
  return std::to_string(left);
}

template <>
std::string Cast::Operation(int64_t left) {
  return std::to_string(left);
}

template <>
std::string Cast::Operation(uint64_t left) {
  return std::to_string(left);
}

template <>
std::string Cast::Operation(double left) {
  return std::to_string(left);
}

template <>
const char* Cast::Operation(int8_t left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* Cast::Operation(int16_t left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* Cast::Operation(int left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* Cast::Operation(int64_t left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* Cast::Operation(uint64_t left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* Cast::Operation(double left) {
  return str_to_cstr(std::to_string(left));
}

template <>
const char* CastFromDate::Operation(date_t left) {
  auto conv = Date::ToString(left);
  auto ret = new char[conv.size() + 1];
  strcpy(ret, conv.c_str());
  return ret;
}

template <>
date_t CastToDate::Operation(const char* left) {
  return Date::FromString(left);
}

}  // namespace Operators

}  // namespace SummerDB