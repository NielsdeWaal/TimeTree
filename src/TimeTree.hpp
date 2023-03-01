#ifndef TIMETREE_H_
#define TIMETREE_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fmt/format.h>
#include <iterator>
#include <limits>
#include <memory>
#include <span>
#include <tl/expected.hpp>
#include <type_traits>
#include <variant>
#include <vector>

enum Errors_e { INVALID_TIME_RANGE, NON_LEAF_PTR_INSERT, RANGE_NOT_IN_DB };

struct TimeRange_t {
  uint64_t start;
  uint64_t end;
  uint64_t ptr;
};


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

    // fmt::print("Constructing {}\n", (leaf ? "leaf" : "node"));

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
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    children.at(m_aryCounter) = {start, end, ptr};
    m_stats.end = end;
    m_aryCounter += 1;
    return m_aryCounter;
  }

  [[nodiscard]] tl::expected<uint64_t, Errors_e> UpdateTimeRange(uint64_t start, uint64_t end) {
    if (start > end) {
      return tl::make_unexpected(Errors_e::INVALID_TIME_RANGE);
    }
    // TODO statistics
    m_stats.start = start;
    m_stats.end = end;
    return m_stats.end;
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
  void SetNodeEnd(uint64_t end) {
    m_stats.end = end;
  }

  [[nodiscard]] std::size_t GetChildCount() const {
    return m_aryCounter;
  }

  TimeTreeNode<arity>* GetLink() const {
    return m_backLink;
  }
  // [[nodiscard]] TimeTreeNode<arity>* GetNewestChild() {
  //   return m_children.at(m_aryCounter - 1);
  // }
  void UpdateNodeEnd() {
    auto& children = std::get<std::array<TimeTreeNode<arity>*, arity>>(m_children);
    TimeTreeNode<arity>* child = children.at(m_aryCounter - 1);
    m_stats.end = child->GetNodeEnd();
  }

  void UpdateNodeStart() {
    auto& children = std::get<std::array<TimeTreeNode<arity>*, arity>>(m_children);
    TimeTreeNode<arity>* child = children.at(0);
    m_stats.start = child->GetNodeStart();
  }

  void SetBackLink(TimeTreeNode<arity>* link) {
    m_backLink = link;
  }

  bool IsLeaf() const {
    return m_leaf;
  }

  // FIXME switch to span
  [[nodiscard]] std::span<TimeRange_t> GetData() {
    assert(m_leaf);
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    return std::span<TimeRange_t>(children.data(), m_aryCounter);
  }

  [[nodiscard]] std::span<TimeTreeNode<arity>*> GetChildren() {
    assert(!m_leaf);
    auto& children = std::get<std::array<TimeTreeNode<arity>*, arity>>(m_children);
    return std::span<TimeTreeNode<arity>*>(children.data(), m_aryCounter);
  }
  // auto& children = std::get<std::array<TimeTreeNode<arity>*, arity>>(m_children);
  // auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
  // return std::make_pair(&children, m_aryCounter);

  std::array<TimeRange_t, arity>::iterator begin() {
    assert(m_leaf);
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    return children.begin();
  }
  std::array<TimeRange_t, arity>::iterator end() {
    assert(m_leaf);
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    return children.end();
  }

  std::array<TimeRange_t, arity>::iterator cbegin() const {
    assert(m_leaf);
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    return children.cbegin();
  }
  std::array<TimeRange_t, arity>::iterator cend() const {
    assert(m_leaf);
    auto& children = std::get<std::array<TimeRange_t, arity>>(m_children);
    return children.cend();
  }

private:
  struct Statistics_t {
    std::size_t min;
    std::size_t max;
    uint64_t start;
    uint64_t end;
  };

  using Children = std::variant<std::array<TimeRange_t, arity>, std::array<TimeTreeNode<arity>*, arity>>;

  bool m_leaf{};
  Statistics_t m_stats;
  // TimeRange_t m_time;


  Children m_children;
  // std::unique_ptr<TimeTreeNode> m_parent;
  TimeTreeNode* m_parent;
  TimeTreeNode* m_backLink{nullptr};

  std::size_t m_aryCounter{0};
};

static_assert(std::is_trivially_copy_assignable_v<TimeRange_t>, "message");

// static_assert(
//     std::is_trivially_constructible_v<TimeTreeNode<8>, bool, uint64_t, uint64_t>,
//     "TimeTreeNode should be trivially constructable");

// TODO iterator which only returns TimeRange_t's from the leafs
template<std::size_t arity>
class TimeTree {
public:
  TimeTree()
  : m_root(std::make_unique<TimeTreeNode<arity>>(true, 0, std::numeric_limits<uint64_t>::max(), 0).release()) {
    m_nodes.push_front({m_root});
  }

  void Insert(uint64_t start, uint64_t end, uint64_t ptr) {
    if (m_aryCounter == arity) {
      std::unique_ptr<TimeTreeNode<arity>> newLeaf = std::make_unique<TimeTreeNode<arity>>(true, start, end, 0);
      const std::size_t height = m_nodes.size();
      auto& leafs = m_nodes.front();
      m_aryCounter = 0;
      // newLeaf->SetBackLink(leafs.back());
      leafs.back()->SetBackLink(newLeaf.get());
      // m_nodes.at(0) = newLeaf.get();
      leafs.push_back(newLeaf.release());
      UpdateTreeLevels(m_nodes.front(), (height > 1) ? std::next(m_nodes.begin()) : m_nodes.end());
    }

    auto& leafs = m_nodes.front();
    auto& newestLeaf = leafs.back();
    m_aryCounter += 1;
    auto insertRet = newestLeaf->Insert(start, end, ptr);
    if (!insertRet) {
      // fmt::print("Failed to insert values: {}\n", insertRet.error());
    }

    if (newestLeaf->GetNodeStart() == 0) {
      newestLeaf->UpdateTimeRange(start, start);
    } else {
      // newestLeaf->UpdateTimeRange(newestLeaf->GetNodeStart(), end);
    }

    UpdateTreeStats();
  }

  [[nodiscard]] std::size_t GetHeight() const {
    return m_nodes.size();
  }

  [[nodiscard]] TimeTreeNode<arity>* GetRoot() const {
    return m_root;
  }

  [[nodiscard]] auto GetLeafsIterator() const {
    return m_nodes.front().begin();
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

  auto begin() {
    return m_nodes.front().begin();
  }
  auto end() {
    return m_nodes.front().end();
  }

  auto cbegin() const {
    return m_nodes.front().cbegin();
  }
  auto cend() const {
    return m_nodes.front().cend();
  }

  tl::expected<std::vector<TimeRange_t>, Errors_e> Query(uint64_t start, uint64_t end) {
    if (start > end) {
      return tl::unexpected(Errors_e::INVALID_TIME_RANGE);
    }
    std::vector<TimeRange_t> res;

    // TimeTreeNode<arity>* node = FindEndOfRange(m_root, end);
    TimeTreeNode<arity>* node = FindStartOfRange(m_root, start);
    // while (!node->IsLeaf()) {
    //   node = FindChildInRange(node, end);
    //   if (node == nullptr) {
    //     return tl::unexpected(Errors_e::RANGE_NOT_IN_DB);
    //   }
    // }

    if (node == nullptr) {
      return tl::unexpected(Errors_e::RANGE_NOT_IN_DB);
    }

    fmt::print("Found end at: {} ({} - {})\n", fmt::ptr(node), node->GetNodeStart(), node->GetNodeEnd());

    // CollectEntries(res, node, start);
    CollectEntries(res, node, end);

    for (const auto val : res) {
      fmt::print("{} -> {}\n", val.start, val.end);
    }
    // while (node->GetNodeStart() >= start) {
    //   for (TimeRange_t ptr : node->GetData()) {
    //     if (!(ptr.start <= start && start <= ptr.end)) {
    //       fmt::print("New timerange : {} - {}\n", ptr.start, ptr.end);
    //       res.push_back(&ptr);
    //     }
    //     // if (ptr.start < start) {
    //     //   return res;
    //     // }
    //   }
    //   // res.push_back(node);
    //   if (node->GetLink() == nullptr) {
    //     break;
    //   }
    //   node = node->GetLink();
    // }

    return res;
  }

  // struct Iterator {
  //   using iterator_category = std::forward_iterator_tag;
  //   using difference_type = std::ptrdiff_t;
  //   using value_type = TimeTreeNode<arity>;
  //   using pointer = TimeTreeNode<arity>*;
  //   using reference = TimeTreeNode<arity>&;
  //   using TimeTreeType = std::deque<TimeTreeNode<arity>*>;

  //   Iterator(TimeTreeType::iterator ptr) : m_ptr(ptr) {}

  //   reference operator*() const {return *}

  // private:
  //   TimeTreeType::iterator m_iter;
  // };

private:
  using ListIter = typename std::deque<std::deque<TimeTreeNode<arity>*>>::iterator;

  void CollectEntries(std::vector<TimeRange_t>& results, TimeTreeNode<arity>* current, uint64_t start) {
    // if (current->GetNodeStart() >= start) {
    //   return;
    // }
    for (TimeRange_t ptr : current->GetData()) {
      if (ptr.start <= start && start <= ptr.end) {
        fmt::print("Found {} -> {}\n", ptr.start, ptr.end);
        results.push_back(ptr);
        return;
      }
      fmt::print("Found {} -> {}\n", ptr.start, ptr.end);
      results.push_back(ptr);
    }

    if (current->GetLink() == nullptr) {
      return;
    }

    CollectEntries(results, current->GetLink(), start);
  }

  TimeTreeNode<arity>* FindChildInRange(TimeTreeNode<arity>* node, uint64_t end) {
    for (auto child : node->GetChildren()) {
      if (child->GetNodeStart() <= end && end <= child->GetNodeEnd()) {
        return child;
      }
    }
    return nullptr;
  }

  TimeTreeNode<arity>* FindEndOfRange(TimeTreeNode<arity>* node, uint64_t end) {
    fmt::print("Search for end: {}\n", end);
    if (node->IsLeaf()) {
      return node;
    }
    TimeTreeNode<arity>* res = nullptr;
    for (TimeTreeNode<arity>* child : node->GetChildren()) {
      fmt::print("Query -> node {} -> {}\n", child->GetNodeStart(), child->GetNodeEnd());
      if (child->GetNodeStart() <= end && end <= child->GetNodeEnd()) {
        fmt::print("Child found\n");
        res = child;
        break;
      }
    }
    if (res == nullptr) {
      return nullptr;
    }
    return FindEndOfRange(res, end);
  }

  TimeTreeNode<arity>* FindStartOfRange(TimeTreeNode<arity>* node, uint64_t start) {
    fmt::print("Search for start: {}\n", start);
    if (node->IsLeaf()) {
      return node;
    }
    TimeTreeNode<arity>* res = nullptr;
    for (TimeTreeNode<arity>* child : node->GetChildren()) {
      fmt::print("Query -> node {} -> {}\n", child->GetNodeStart(), child->GetNodeEnd());
      if (child->GetNodeStart() <= start && start <= child->GetNodeEnd()) {
        fmt::print("Child found\n");
        res = child;
        break;
      }
    }
    if (res == nullptr) {
      return nullptr;
    }
    return FindStartOfRange(res, start);
  }

  void UpdateTreeStats() {
    // for (const auto& level : m_nodes) {
    //   TimeTreeNode<arity>* newestLeaf = level.back();
    ListIter iter = std::next(m_nodes.begin());
    for (; iter != m_nodes.end(); ++iter) {
      TimeTreeNode<arity>* newestParent = iter->back();
      newestParent->UpdateNodeEnd();
      // uint64_t end = newestParent->GetChildren().at(newestParent->GetChildCount())->GetNodeEnd();
      // newestParent->SetNodeEnd(end);
    }
    // ListIter iter = std::next(m_nodes.begin());
    // for (; iter != m_nodes.end(); ++iter) {
    //   // // Another option might be to get the n of child nodes for the parent and take
    //   // // the last n nodes of the iter level
    //   // std::deque<TimeTreeNode<arity>>& parent = iter->back();
    //   // std::array<TimeTreeNode<arity>*, arity>& children = parent.back().GetChildren();
    //   auto parentLevel = std::next(iter);
    //   auto& children = parentLevel->back()->GetChildren();

    //   parentLevel->back()->SetNodeEnd(children.back()->GetNodeEnd());

    //   // auto res = std::max_element(children.begin(), children.end(), )
    // }
  }

  /**
   * @brief update the tree levels
   *
   * - Creates new parents if new node is orphaned
   * - Link parent of new node
   * - Create new root if root is full
   * */
  void UpdateTreeLevels(std::deque<TimeTreeNode<arity>*>& childList, ListIter rest, std::size_t level = 0) {
    if (rest == m_nodes.end()) {
      // fmt::print("Updating root level\n");
      const std::size_t rootLength = childList.size();
      if (rootLength > 1) {
        // fmt::print("Constructing new root\n");
        const std::size_t offset = std::min(arity, rootLength);
        const uint64_t tsStart = (*std::prev(childList.end(), offset))->GetNodeStart();
        const uint64_t tsEnd = childList.back()->GetNodeEnd();
        auto newRoot = std::make_unique<TimeTreeNode<arity>>(false, tsStart, tsEnd, 0);

        m_root = newRoot.release();
        m_nodes.push_back({m_root});

        for (auto it = std::prev(childList.end(), offset); it != childList.end(); ++it) {
          m_root->InsertChild(*it);
        }

        // m_root->UpdateNodeStart();

        UpdateTreeLevels(m_nodes.back(), m_nodes.end(), ++level);
      }
    } else {
      // fmt::print("Updating non-root level\n");
      const auto& parentLevel = *rest;
      const auto& rightMostParent = parentLevel.back();
      const std::size_t parentChildCount = rightMostParent->GetChildCount();
      const auto& leftMostChild = childList.back();

      if (parentChildCount >= arity) {
        // fmt::print("Inserting new parent\n");
        auto newNode =
            std::make_unique<TimeTreeNode<arity>>(false, leftMostChild->GetNodeStart(), leftMostChild->GetNodeEnd(), 0);

        m_nodes.at(level + 1).push_back(newNode.release());
        UpdateTreeLevels(*rest, std::next(rest), level + 1);
        m_nodes.at(level + 1).back()->InsertChild(leftMostChild);
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
