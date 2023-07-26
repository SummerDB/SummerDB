#ifndef SUMMERDB_CATALOG_CATALOG_SET_HPP
#define SUMMERDB_CATALOG_CATALOG_SET_HPP

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Transaction/Transaction.hpp"

namespace SummerDB {

//! The Catalog Set stores (key, value) map of a set of AbstractCatalogEntries
class CatalogSet {
 public:
  //! Create an entry in the catalog set. Returns whether or not it was
  //! successful.
  bool CreateEntry(Transaction& transaction, const std::string& name,
                   std::unique_ptr<AbstractCatalogEntry> value);

  bool DropEntry(Transaction& transaction, const std::string& name);
  //! Returns whether or not an entry exists
  bool EntryExists(Transaction& transaction, const std::string& name);
  //! Returns the entry with the specified name
  AbstractCatalogEntry* GetEntry(Transaction& transaction,
                                 const std::string& name);

  //! Rollback <entry> to be the currently valid entry for a certain catalog
  //! entry
  void Undo(AbstractCatalogEntry* entry);

 private:
  //! The catalog lock is used to make changes to the data
  std::mutex catalog_lock;
  //! The set of entries present in the CatalogSet.
  std::unordered_map<std::string, std::unique_ptr<AbstractCatalogEntry>> data;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_CATALOG_SET_HPP