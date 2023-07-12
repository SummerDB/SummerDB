#ifndef SUMMERDB_COMMON_PRINTABLE_HPP
#define SUMMERDB_COMMON_PRINTABLE_HPP

#include <string>

#include "SummerDB/Common/Helper.hpp"

namespace SummerDB {

class Deserializer;
class Serializer;

//! Printable is an abstract class that represents an object that can be
//! converted to a string for logging and debugging purposes.
class Printable {
 public:
  virtual ~Printable(){};

  //! Convert the object to a string
  virtual std::string ToString() const = 0;

  //! Print the object to stderr
  void Print();
};

}  // namespace SummerDB

#endif  // SUMMERDB_COMMON_PRINTABLE_HPP