#include "SummerDB/Storage/DataTable.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/Helper.hpp"
#include "SummerDB/Storage/DataColumn.hpp"

namespace SummerDB {

void DataTable::AddColumn(ColumnCatalogEntry& column) {
  columns.push_back(make_unique<DataColumn>(*this, column));
}

void DataTable::AddData(DataChunk& chunk) {
  if (chunk.column_count != columns.size()) {
    throw CatalogException("Mismatch in column count for append");
  }
  for (size_t i = 0; i < chunk.column_count; i++) {
    columns[i]->AddData(chunk.data[i]);
  }
  size += chunk.count;
}

}  // namespace SummerDB