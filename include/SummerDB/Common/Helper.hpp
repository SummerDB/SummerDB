#ifndef SUMMERDB_COMMON_HELPER_HPP
#define SUMMERDB_COMMON_HELPER_HPP

#include <limits>
#include <memory>
#include <sstream>

namespace SummerDB {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename S, typename T, typename... Args>
std::unique_ptr<S> make_unique_base(Args&&... args) {
  return std::unique_ptr<S>(new T(std::forward<Args>(args)...));
}

template <class T>
inline bool in_bounds(int64_t value) {
  return value >= std::numeric_limits<T>::min() &&
         value <= std::numeric_limits<T>::max();
}

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_HELPER_HPP