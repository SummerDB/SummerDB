#include "SummerDB/Common/Serializer.hpp"

namespace SummerDB {

Serializer::Serializer(size_t maximum_size)
    : Serializer(std::unique_ptr<uint8_t[]>(new uint8_t[maximum_size]),
                 maximum_size) {}

Serializer::Serializer(std::unique_ptr<uint8_t[]> data, size_t size)
    : maximum_size(size), data(data.get()) {
  blob.size = 0;
  blob.data = std::move(data);
}

Serializer::Serializer(uint8_t* data) : maximum_size((size_t)-1), data(data) {
  blob.size = 0;
}

Deserializer::Deserializer(uint8_t* ptr, size_t data)
    : ptr(ptr), endptr(ptr + data) {}

template <>
std::string Deserializer::Read(bool& failed) {
  auto size = Read<uint32_t>(failed);
  if (ptr + size > endptr) {
    failed = true;
    return std::string();
  }
  auto value = std::string((char*)ptr, size);
  ptr += size;
  return value;
}

}  // namespace SummerDB