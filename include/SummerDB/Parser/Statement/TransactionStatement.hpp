#ifndef SUMMERDB_PARSER_STATEMENT_TRANSACTION_STATEMENT_HPP
#define SUMMERDB_PARSER_STATEMENT_TRANSACTION_STATEMENT_HPP

#include <vector>

#include "SummerDB/Parser/Expression.hpp"
#include "SummerDB/Parser/SqlStatement.hpp"

namespace SummerDB {

class TransactionStatement : public SQLStatement {
 public:
  TransactionStatement(TransactionType type)
      : SQLStatement(StatementType::TRANSACTION), type(type){};
  virtual ~TransactionStatement() {}
  virtual std::string ToString() const { return "Transaction"; }
  virtual void Accept(SQLNodeVisitor* v) { v->Visit(*this); }

  TransactionType type;
};

}  // namespace SummerDB

#endif  // SUMMERDB_PARSER_STATEMENT_TRANSACTION_STATEMENT_HPP