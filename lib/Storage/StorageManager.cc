#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

void StorageManager::CreateTable(TableCatalogEntry& table) {
  auto storage = make_unique<DataTable>(*this, table);
  tables.push_back(std::move(storage));
  table.storage = tables.back().get();
}

}  // namespace SummerDB