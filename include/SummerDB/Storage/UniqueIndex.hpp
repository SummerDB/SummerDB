#ifndef SUMMERDB_STORAGE_UNIQUE_INDEX_HPP
#define SUMMERDB_STORAGE_UNIQUE_INDEX_HPP

#include <mutex>
#include <unordered_set>
#include <vector>

#include "SummerDB/Common/Types/DataChunk.hpp"
#include "SummerDB/Common/Types/Tuple.hpp"
#include "SummerDB/Storage/StorageChunk.hpp"

namespace SummerDB {

class DataTable;
class Transaction;

struct UniqueIndexNode {
  Tuple tuple;
  size_t row_identifier;

  UniqueIndexNode* parent;
  std::unique_ptr<UniqueIndexNode> left;
  std::unique_ptr<UniqueIndexNode> right;

  UniqueIndexNode(Tuple tuple, size_t row_identifier)
      : tuple(std::move(tuple)),
        row_identifier(row_identifier),
        parent(nullptr) {}
};

//! The unique index is used to lookup whether or not multiple values have the
//! same value. It is used to efficiently enforce PRIMARY KEY, FOREIGN KEY and
//! UNIQUE constraints.
class UniqueIndex {
 public:
  UniqueIndex(DataTable& table, std::vector<TypeId> types,
              std::vector<size_t> keys, bool allow_nulls);

  static std::string Append(Transaction& transaction,
                            std::vector<std::unique_ptr<UniqueIndex>>& indexes,
                            DataChunk& chunk, size_t row_identifier_start);

  static std::string Update(Transaction& transaction, StorageChunk* storage,
                            std::vector<std::unique_ptr<UniqueIndex>>& indexes,
                            std::vector<column_t>& column_ids,
                            DataChunk& update_chunk, Vector& row_identifiers);

 private:
  std::string AddEntries(Transaction& transaction,
                         UniqueIndexNode* added_nodes[], Tuple tuples[],
                         bool has_null[], Vector& row_identifiers,
                         std::unordered_set<size_t>& ignored_identifiers);
  UniqueIndexNode* AddEntry(Transaction& transaction, Tuple tuple,
                            size_t row_identifier,
                            std::unordered_set<size_t>& ignored_identifiers);
  void RemoveEntry(UniqueIndexNode* entry);

  //! The tuple serializer
  TupleSerializer serializer;
  //! The comparer used to compare tuples stored in the index to tuples of the
  //! base table
  TupleComparer comparer;
  //! A reference to the table this Unique constraint relates to
  DataTable& table;
  //! Types of the UniqueIndex
  std::vector<TypeId> types;
  //! The set of keys that must be collectively unique
  std::vector<size_t> keys;
  //! Lock on the index
  std::mutex index_lock;
  //! Whether or not NULL values are allowed by the constraint (false for
  //! PRIMARY KEY, true for UNIQUE)
  bool allow_nulls;

  //! Root node of the index
  std::unique_ptr<UniqueIndexNode> root;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_UNIQUE_INDEX_HPP