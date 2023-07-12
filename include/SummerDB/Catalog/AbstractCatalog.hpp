#ifndef SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP
#define SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP

#include <memory>

#include "SummerDB/Common/Printable.hpp"

namespace SummerDB {

class Catalog;

//! Abstract base class of an entry in the catalog
class AbstractCatalogEntry : public Printable {
 public:
  AbstractCatalogEntry(Catalog* catalog, std::string name)
      : catalog(catalog), name(name) {}

  virtual ~AbstractCatalogEntry() {}

  //! Reference to the catalog this entry belongs to
  Catalog* catalog;
  //! The name of the entry
  std::string name;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP