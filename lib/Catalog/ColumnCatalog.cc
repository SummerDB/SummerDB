#include "SummerDB/Catalog/ColumnCatalog.hpp"

namespace SummerDB {

ColumnDefinition::ColumnDefinition(std::string name, TypeId type)
    : name(name), type(type), has_default(false) {}

ColumnDefinition::ColumnDefinition(std::string name, TypeId type,
                                   Value default_value)
    : name(name), type(type), has_default(true), default_value(default_value) {}

}  // namespace SummerDB