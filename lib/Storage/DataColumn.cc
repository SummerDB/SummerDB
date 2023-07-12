#include "SummerDB/Storage/DataColumn.hpp"

#include "SummerDB/Common/Exception.hpp"
#include "SummerDB/Common/Helper.hpp"
#include "SummerDB/Storage/StorageManager.hpp"

namespace SummerDB {

void DataColumn::AddData(Vector& data) {
  if (data.type != column.type) {
    throw CatalogException("Mismatch in column type");
  }
  if (this->data.size() > 0) {
    auto& back = this->data.back();
    back->Append(data);
    return;
  }
  this->data.push_back(make_unique<Vector>());
  // base tables need to own the data
  // if <data> owns the data we can take it
  // otherwise we need to make a copy
  data.Move(*this->data.back());
  this->data.back()->ForceOwnership();
}

}  // namespace SummerDB