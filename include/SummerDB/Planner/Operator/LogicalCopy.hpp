#ifndef SUMMERDB_PLANNER_OPERATOR_LOGICAL_COPY_HPP
#define SUMMERDB_PLANNER_OPERATOR_LOGICAL_COPY_HPP

#include "SummerDB/Planner/LogicalOperator.hpp"

namespace SummerDB {

class LogicalCopy : public LogicalOperator {
 public:
  LogicalCopy(TableCatalogEntry* table, std::string file_path, bool is_from,
              char delimiter, char quote, char escape,
              std::vector<std::string> select_list)
      : LogicalOperator(LogicalOperatorType::COPY),
        table(table),
        file_path(file_path),
        select_list(select_list),
        is_from(is_from),
        delimiter(delimiter),
        quote(quote),
        escape(escape) {}
  LogicalCopy(std::string file_path, bool is_from, char delimiter, char quote,
              char escape)
      : LogicalOperator(LogicalOperatorType::COPY),
        file_path(file_path),
        is_from(is_from),
        delimiter(delimiter),
        quote(quote),
        escape(escape) {}

  virtual void Accept(LogicalOperatorVisitor* v) override { v->Visit(*this); }

  TableCatalogEntry* table;

  std::string file_path;

  std::vector<std::string> select_list;

  bool is_from;

  char delimiter = ',';
  char quote = '"';
  char escape = '"';
};

}  // namespace SummerDB

#endif  // SUMMERDB_PLANNER_OPERATOR_LOGICAL_COPY_HPP