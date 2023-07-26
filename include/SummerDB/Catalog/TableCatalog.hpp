#ifndef SUMMERDB_CATALOG_TABLE_CATALOG_HPP
#define SUMMERDB_CATALOG_TABLE_CATALOG_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "SummerDB/Catalog/AbstractCatalog.hpp"
#include "SummerDB/Catalog/ColumnDefinition.hpp"
#include "SummerDB/Common/Types/Statistics.hpp"
#include "SummerDB/Parser/Constraint.hpp"
#include "SummerDB/Parser/ParsedData.hpp"

namespace SummerDB {

class DataTable;
class SchemaCatalogEntry;

//! A table catalog entry
class TableCatalogEntry : public AbstractCatalogEntry {
 public:
  //! Create a real TableCatalogEntry and initialize storage for it
  TableCatalogEntry(Catalog* catalog, SchemaCatalogEntry* schema,
                    CreateTableInformation* info);

  //! The schema the table belongs to
  SchemaCatalogEntry* schema;
  //! A reference to the underlying storage unit used for this table
  std::unique_ptr<DataTable> storage;
  //! A list of columns that are part of this table
  std::vector<ColumnDefinition> columns;
  //! A list of constraints that are part of this table
  std::vector<std::unique_ptr<Constraint>> constraints;
  //! A map of column name to column index
  std::unordered_map<std::string, column_t> name_map;

  //! Adds a column to this table
  // void AddColumn(ColumnDefinition entry);
  //! Returns whether or not a column with the given name exists
  bool ColumnExists(const std::string& name);
  //! Returns the statistics of the oid-th column. Throws an exception if the
  //! access is out of range.
  Statistics& GetStatistics(column_t oid);
  //! Returns a reference to the column of the specified name. Throws an
  //! exception if the column does not exist.
  ColumnDefinition& GetColumn(const std::string& name);
  //! Returns a list of types of the table
  std::vector<TypeId> GetTypes();
};

}  // namespace SummerDB

#endif  // SUMMERDB_CATALOG_TABLE_CATALOG_HPP