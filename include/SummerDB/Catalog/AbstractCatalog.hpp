#ifndef SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP
#define SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP

#include <memory>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"

namespace SummerDB {

class Catalog;
class CatalogSet;

//! Abstract base class of an entry in the catalog
class AbstractCatalogEntry {
 public:
  AbstractCatalogEntry(CatalogType type, Catalog* catalog, std::string name)
      : type(type),
        catalog(catalog),
        set(nullptr),
        name(name),
        deleted(false),
        parent(nullptr) {}

  virtual ~AbstractCatalogEntry() {}

  //! The type of this catalog entry
  CatalogType type;
  //! Reference to the catalog this entry belongs to
  Catalog* catalog;
  //! Reference to the catalog set this entry is stored in
  CatalogSet* set;
  //! The name of the entry
  std::string name;
  //! Whether or not the object is deleted
  bool deleted;
  //! Timestamp at which the catalog entry was created
  transaction_t timestamp;
  //! Child entry
  std::unique_ptr<AbstractCatalogEntry> child;
  //! Parent entry (the node that owns this node)
  AbstractCatalogEntry* parent;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_ABSRACT_CATALOG_HPP