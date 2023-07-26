#ifndef SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_COPY_HPP
#define SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_COPY_HPP

#include <fstream>

#include "SummerDB/Execution/PhysicalOperator.hpp"

namespace SummerDB {

//! Physically copy file into a table
class PhysicalCopy : public PhysicalOperator {
 public:
  PhysicalCopy(TableCatalogEntry* table, std::string file_path, bool is_from,
               char delimiter, char quote, char escape,
               std::vector<std::string> select_list)
      : PhysicalOperator(PhysicalOperatorType::COPY),
        table(table),
        file_path(file_path),
        is_from(is_from),
        select_list(select_list),
        delimiter(delimiter),
        quote(quote),
        escape(escape) {}

  PhysicalCopy(std::string file_path, bool is_from, char delimiter, char quote,
               char escape)
      : PhysicalOperator(PhysicalOperatorType::COPY),
        table(nullptr),
        file_path(file_path),
        is_from(is_from),
        delimiter(delimiter),
        quote(quote),
        escape(escape) {}
  std::vector<TypeId> GetTypes() override;
  virtual void _GetChunk(ClientContext& context, DataChunk& chunk,
                         PhysicalOperatorState* state) override;

  virtual std::unique_ptr<PhysicalOperatorState> GetOperatorState(
      ExpressionExecutor* executor) override;

  TableCatalogEntry* table;
  std::string file_path;
  bool is_from;
  std::vector<std::string> select_list;

  char delimiter = ',';
  char quote = '"';
  char escape = '"';
};

}  // namespace SummerDB

#endif  // SUMMERDB_EXECUTION_OPERATOR_PHYSICAL_COPY_HPP