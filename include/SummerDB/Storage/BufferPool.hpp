#ifndef SUMMERDB_STORAGE_BUFFER_POOL_HPP
#define SUMMERDB_STORAGE_BUFFER_POOL_HPP

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "SummerDB/Storage/Page.hpp"

namespace SummerDB {

class BufferPool {
  Page* CreatePage();
  void LoadPage(int64_t page_identifier);

  std::mutex loading_lock;
  std::unordered_map<int64_t, Page*> loading_pages;
  // PageQueue cooling_queue;
};

}  // namespace SummerDB

#endif  // SUMMERDB_STORAGE_BUFFER_POOL_HPP