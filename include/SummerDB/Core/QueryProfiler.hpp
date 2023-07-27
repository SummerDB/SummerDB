#ifndef SUMMERDB_CORE_QUERY_PROFILER_HPP
#define SUMMERDB_CORE_QUERY_PROFILER_HPP

#include <stack>
#include <unordered_map>

#include "SummerDB/Common/InternalTypes.hpp"
#include "SummerDB/Common/Printable.hpp"
#include "SummerDB/Common/Profiler.hpp"
#include "SummerDB/Common/StringUtil.hpp"
#include "SummerDB/Common/Types/DataChunk.hpp"

namespace SummerDB {

class PhysicalOperator;

//! The QueryProfiler can be used to measure timings of queries
class QueryProfiler : public Printable {
 public:
  struct TimingInformation {
    double time = 0;
    size_t elements = 0;

    TimingInformation() : time(0), elements(0) {}
  };
  struct TreeNode {
    std::string name;
    TimingInformation info;
    std::vector<std::unique_ptr<TreeNode>> children;
    size_t depth = 0;
  };

 private:
  static size_t GetDepth(TreeNode& node);
  std::unique_ptr<TreeNode> CreateTree(PhysicalOperator* root,
                                       size_t depth = 0);

  static size_t RenderTreeRecursive(TreeNode& node,
                                    std::vector<std::string>& render,
                                    size_t base_render_x);
  static std::string RenderTree(TreeNode& node);

 public:
  QueryProfiler() : enabled(false) {}

  void Enable() { enabled = true; }

  void Disable() { enabled = false; }

  void StartQuery(std::string query);
  void EndQuery();

  void StartOperator(PhysicalOperator* phys_op);
  void EndOperator(DataChunk& chunk);

  virtual std::string ToString() const override;

 private:
  bool enabled;

  std::unique_ptr<TreeNode> root;
  std::string query;

  Profiler main_query;
  Profiler op;
  std::unordered_map<PhysicalOperator*, TreeNode*> tree_map;
  std::stack<PhysicalOperator*> execution_stack;
};

}  // namespace SummerDB

#endif  // SUMMERDB_CORE_QUERY_PROFILER_HPP