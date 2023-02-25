#ifndef TIMETREE_H_
#define TIMETREE_H_

#include <cstddef>
#include <cstdint>
#include <deque>
#include <fmt/format.h>
#include <limits>
#include <memory>
#include <tl/expected.hpp>
#include <variant>

enum Errors_e { INVALID_TIME_RANGE, NON_LEAF_PTR_INSERT };

template<std::size_t arity>
class TimeTreeNode {
public:
  TimeTreeNode() = default;

  // , m_time({start, end})
  TimeTreeNode(bool leaf, uint64_t start, uint64_t end, uint64_t ptr, TimeTreeNode<arity>* parent = nullptr)
  : m_leaf(leaf) {
    if (parent != nullptr) {
      m_parent = parent;
    }

    fmt::print("Constructing {}\n", (leaf ? "leaf" : "node"));

    if (leaf) {
      m_children = std::array<TimeRange_t, arity>{{start, end, ptr}};
    } else {
      m_children = std::array<TimeTreeNode<arity>*, arity>{};
    }

    m_stats.start = start;
    m_stats.end = end;
  }

  [[nodiscard]] tl::expected<int, Errors_e> Insert(uint64_t start, uint64_t end, uint64_t ptr) {
    if (start > end) {
      return tl::make_unexpected(Errors_e::INVALID_TIME_RANGE);
    }
    // if (m_stats.end > start) {
    //   return tl::make_unexpected(Errors_e::INVALID_TIME_RANGE);
    // }
    if (!m_leaf) {
      return tl::make_unexpected(Errors_e::NON_LEAF_PTR_INSERT);
    }
    m_stats.end = end;
    m_aryCounter += 1;
    return m_aryCounter;
  }

  [[nodiscard]] tl::expected<void, Errors_e> UpdateTimeRange(uint64_t start, uint64_t end) {
    if (start > end) {
      return tl::make_unexpected(Errors_e::INVALID_TIME_RANGE);
    }
    // TODO statistics
  }

  void InsertChild(TimeTreeNode<arity>* child) {
    // assert(m_aryCounter < arity);
    auto& children = std::get<std::array<TimeTreeNode<arity>*, arity>>(m_children);
    children.at(m_aryCounter) = child;
    m_aryCounter += 1;
  }

  [[nodiscard]] uint64_t GetNodeStart() const {
    return m_stats.start;
  }

  [[nodiscard]] uint64_t GetNodeEnd() const {
    return m_stats.end;
  }

  [[nodiscard]] std::size_t GetChildCount() const {
    return m_aryCounter;
  }

private:
  struct Statistics_t {
    std::size_t min;
    std::size_t max;
    uint64_t start;
    uint64_t end;
  };

  struct TimeRange_t {
    uint64_t start;
    uint64_t end;
    uint64_t ptr;
  };

  using Children = std::variant<std::array<TimeRange_t, arity>, std::array<TimeTreeNode<arity>*, arity>>;

  bool m_leaf{};
  Statistics_t m_stats;
  // TimeRange_t m_time;


  Children m_children;
  // std::unique_ptr<TimeTreeNode> m_parent;
  TimeTreeNode* m_parent;

  std::size_t m_aryCounter{0};
};

template<std::size_t arity>
class TimeTree {
public:
  TimeTree()
  : m_root(std::make_unique<TimeTreeNode<arity>>(true, 0, std::numeric_limits<uint64_t>::max(), 0).release()) {
    m_nodes.push_front({m_root});
  }

  void Insert(uint64_t start, uint64_t end, uint64_t ptr) {
    if (m_aryCounter == arity) {
      std::unique_ptr<TimeTreeNode<arity>> newLeaf =
          std::make_unique<TimeTreeNode<arity>>(true, 0, std::numeric_limits<uint64_t>::max(), 0);
      const std::size_t height = m_nodes.size();
      auto& leafs = m_nodes.front();
      m_aryCounter = 0;
      // m_nodes.at(0) = newLeaf.get();
      leafs.push_back(newLeaf.release());
      UpdateTreeLevels(m_nodes.front(), (height > 1) ? std::next(m_nodes.begin()) : m_nodes.end());
      // if (height > 1) {
      //   UpdateTreeLevels(m_nodes.front(), std::next(m_nodes.begin()));
      // } else {
      //   UpdateTreeLevels(m_nodes.front(), m_nodes.end());
      // }
    }

    auto& leafs = m_nodes.front();
    auto& newestLeaf = leafs.front();
    m_aryCounter += 1;
    auto insertRet = newestLeaf->Insert(start, end, ptr);
    if (!insertRet) {
      fmt::print("Failed to insert values: {}\n", insertRet.error());
    }

    UpdateTreeStats();
  }

  [[nodiscard]] std::size_t GetHeight() const {
    return m_nodes.size();
  }

  [[nodiscard]] TimeTreeNode<arity>* GetRoot() const {
    return m_root;
  }

  [[nodiscard]] std::size_t GetNumberLeafs() const {
    return m_nodes.front().size();
  }

  void PrintTree() const {
    for (const auto& level : m_nodes) {
      for (const TimeTreeNode<arity>* node : level) {
        fmt::print("[{} {} {}] ", node->GetNodeStart(), node->GetNodeEnd(), node->GetChildCount());
      }
      fmt::print("\n");
    }
  }

private:
  using ListIter = typename std::deque<std::deque<TimeTreeNode<arity>*>>::iterator;
  void UpdateTreeStats() {}

  /**
   * @brief update the tree levels
   *
   * - Creates new parents if new node is orphaned
   * - Link parent of new node
   * - Create new root if root is full
   * */
  // void UpdateTreeLevels() {
  //   std::size_t level = 0;
  //   auto children = m_nodes.begin();
  //   // Need to make sure there is always a level higher available, final level is root level
  //   for (; children != std::prev(m_nodes.end()); ++children) {
  //     const auto parentLevel = std::next(children);
  //     const auto& rightMostParent = (*parentLevel).front();
  //     const auto& parentChildCount = rightMostParent->GetChildCount();
  //     const auto& leftMostChild = (*children).back();

  //     // If parent has enough room, just add it to children and return from function
  //     if (rightMostParent->GetChildCount() < arity) {
  //       rightMostParent->InsertChild((*children).back());
  //       return;
  //     }

  //     // Need new parent node
  //     std::unique_ptr<TimeTreeNode<arity>> newLeaf =
  //         std::make_unique<TimeTreeNode<arity>>(true, 0, std::numeric_limits<uint64_t>::max(), 0);
  //     std::next(children)->push_back(newLeaf.release());
  //   }
  // }
  /* void UpdateTreeLevels(std::deque<TimeTreeNode<arity>*>& childList, std::size_t level = 0) { */
  // void UpdateTreeLevels(ListIter childList, std::size_t level = 0) {
  //   const std::size_t rootLength = childList.size();
  //   if (rootLength > 1) {
  //     auto newRoot =
  //         std::make_unique<TimeTreeNode<arity>>(false, 0, std::numeric_limits<uint64_t>::max(), 0, m_nodes.back());
  //     // FIXME check if root needs to be released, might cause unique_ptr/moved-from issues
  //     m_nodes.push_back({newRoot.get()});
  //     m_root = newRoot;

  //     UpdateTreeLevels(m_nodes.back(), ++level);
  //   }
  // }

  // TODO change childList and rest to iterator
  // If iterator.next() == iterator.end() we know we've reached the end of the list of nodes
  void UpdateTreeLevels(std::deque<TimeTreeNode<arity>*>& childList, ListIter rest, std::size_t level = 0) {
    if (rest == m_nodes.end()) {
      fmt::print("Updating root level\n");
      const std::size_t rootLength = childList.size();
      if (rootLength > 1) {
        fmt::print("Constructing new root\n");
        auto newRoot = std::make_unique<TimeTreeNode<arity>>(false, 0, std::numeric_limits<uint64_t>::max(), 0);

        m_root = newRoot.release();
        m_nodes.push_back({m_root});

        for (auto it = std::prev(childList.end(), arity); it != childList.end(); ++it) {
          m_root->InsertChild(*it);
        }

        UpdateTreeLevels(m_nodes.back(), m_nodes.end(), ++level);
      }
    } else {
      fmt::print("Updating non-root level\n");
      const auto& parentLevel = *rest;
      const auto& rightMostParent = parentLevel.front();
      const std::size_t parentChildCount = rightMostParent->GetChildCount();
      const auto& leftMostChild = childList.back();

      if (parentChildCount >= arity) {
        fmt::print("Inserting new parent\n");
        auto newNode = std::make_unique<TimeTreeNode<arity>>(false, 0, std::numeric_limits<uint64_t>::max(), 0);

        m_nodes.at(level + 1).push_back(newNode.release());
        UpdateTreeLevels(*rest, std::next(rest), ++level);
      } else {
        rightMostParent->InsertChild(leftMostChild);
      }
    }
  }

  // std::unique_ptr<TimeTreeNode<arity>> m_root;
  TimeTreeNode<arity>* m_root;
  // std::deque<TimeTreeNode<arity>*> m_nodes;
  std::deque<std::deque<TimeTreeNode<arity>*>> m_nodes;

  std::size_t m_aryCounter{0};
};

#endif // TIMETREE_H_
