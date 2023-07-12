#include "SummerDB/Common/Printable.hpp"

#include <stdio.h>

namespace SummerDB {

void Printable::Print() { fprintf(stderr, "%s\n", ToString().c_str()); }

}  // namespace SummerDB