#include "SummerDB/Catalog/ColumnCatalog.hpp"

namespace SummerDB {

ColumnCatalogEntry::ColumnCatalogEntry(std::string name, TypeId type,
                                       bool is_not_null)
    : AbstractCatalogEntry(nullptr, name),
      type(type),
      is_not_null(is_not_null),
      has_default(false) {}

ColumnCatalogEntry::ColumnCatalogEntry(std::string name, TypeId type,
                                       bool is_not_null, Value default_value)
    : AbstractCatalogEntry(nullptr, name),
      type(type),
      is_not_null(is_not_null),
      has_default(true),
      default_value(default_value) {}

}  // namespace SummerDB