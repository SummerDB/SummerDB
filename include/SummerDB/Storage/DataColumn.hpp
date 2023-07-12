#ifndef SUMMERDB_STORAGE_DATA_COLUMN_HPP
#define SUMMERDB_STORAGE_DATA_COLUMN_HPP

#include <vector>

#include "SummerDB/Catalog/TableCatalog.hpp"
#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Types/Vector.hpp"

namespace SummerDB {

class DataTable;

//! DataColumn represents a physical column on disk
class DataColumn {
  public:
	DataColumn(DataTable &table, ColumnCatalogEntry &column)
	    : table(table), column(column) {}

	void AddData(Vector &data);

	//! The physical table that this column belongs to
	DataTable &table;
	//! A reference to the column in the catalog
	ColumnCatalogEntry &column;
	//! The actual data of the column
	std::vector<std::unique_ptr<Vector>> data;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_DATA_COLUMN_HPP