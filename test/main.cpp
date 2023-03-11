#include <cstdint>
#include <deque>
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "src/TimeTree.hpp"

#include <catch2/catch.hpp>

TEST_CASE("Basic tree insertion tests", "[TimeTree]") {
  SECTION("Arity of 2") {
    TimeTree<2> tree;
    REQUIRE(tree.GetHeight() == 1);

    auto startingRoot = tree.GetRoot();

    tree.Insert(1, 1, 0);
    REQUIRE(tree.GetHeight() == 1);
    tree.Insert(2, 2, 0);
    REQUIRE(tree.GetHeight() == 1);
    REQUIRE(startingRoot->GetChildCount() == 2);
    REQUIRE(tree.GetNumberLeafs() == 1);

    tree.Insert(3, 3, 0);
    auto newRoot = tree.GetRoot();

    tree.PrintTree();

    REQUIRE(tree.GetHeight() == 2);
    REQUIRE(tree.GetNumberLeafs() == 2);
    REQUIRE(startingRoot != newRoot);
    REQUIRE(newRoot->GetChildCount() == 2);

    auto iter = tree.GetLeafsIterator();
    TimeTreeNode<2>* start = *iter;
    REQUIRE((*iter)->GetChildCount() == 2);
    REQUIRE(start->GetLink() == (*std::next(iter)));
    REQUIRE((*std::next(iter))->GetChildCount() == 1);

    uint64_t tester = 1;
    for (TimeTreeNode<2>& node : tree) {
      REQUIRE(node.GetNodeStart() != 0);
      REQUIRE(node.GetNodeEnd() != 0);
      const std::span<TimeRange_t> range = node.GetData();
      for (const TimeRange_t r : range) {
        REQUIRE(r.start == tester);
        REQUIRE(r.end == tester);
        REQUIRE(r.ptr == 0);
        ++tester;
      }
      // for (TimeRange_t item : *node) {
      //   REQUIRE(item.start != 0);
      //   REQUIRE(item.end != 0);
      //   REQUIRE(item.ptr == 0);
      // }
    }

    // std::size_t count = (*iter)->GetChildCount();
    // for (std::size_t i = 0; i != count; ++i) {
    //   for (const auto& ptr : (*iter)->GetData()) {
    //     REQUIRE(ptr.start != 0);
    //     REQUIRE(ptr.end != 0);
    //     REQUIRE(ptr.ptr == 0);
    //   }
    //   ++iter;
    // }

    tree.Insert(4, 4, 0);
    tree.Insert(5, 5, 0);
    REQUIRE(tree.GetHeight() == 3);
    tree.Insert(6, 6, 0);
    tree.Insert(7, 7, 0);

    tree.Insert(8, 8, 0);
    tree.Insert(9, 9, 0);
    tree.Insert(10, 10, 0);
    tree.Insert(11, 11, 0);
    tree.PrintTree();
  };

  SECTION("Arity of 8") {
    TimeTree<8> tree;
    REQUIRE(tree.GetHeight() == 1);

    auto startingRoot = tree.GetRoot();

    for (int i = 1; i <= 8; ++i) {
      tree.Insert(i, i, 0);
    }
    REQUIRE(tree.GetHeight() == 1);
    REQUIRE(startingRoot->GetChildCount() == 8);
    REQUIRE(tree.GetNumberLeafs() == 1);

    for (int i = 9; i <= 16; ++i) {
      tree.Insert(i, i, 0);
    }
    REQUIRE(tree.GetHeight() == 2);
    REQUIRE(tree.GetNumberLeafs() == 2);

    uint64_t tester = 1;
    for (TimeTreeNode<8>& node : tree) {
      REQUIRE(node.GetNodeStart() != 0);
      REQUIRE(node.GetNodeEnd() != 0);
      const std::span<TimeRange_t> range = node.GetData();
      for (const TimeRange_t r : range) {
        REQUIRE(r.start == tester);
        REQUIRE(r.end == tester);
        REQUIRE(r.ptr == 0);
        ++tester;
      }
      // for (TimeRange_t item : *node) {
      //   REQUIRE(item.start != 0);
      //   REQUIRE(item.end != 0);
      //   REQUIRE(item.ptr == 0);
      // }
    }
    // for (TimeTreeNode<8>* node : tree) {
    //   REQUIRE(node->GetNodeStart() != 0);
    //   REQUIRE(node->GetNodeEnd() != 0);
    //   for (TimeRange_t item : *node) {
    //     REQUIRE(item.start != 0);
    //     REQUIRE(item.end != 0);
    //     REQUIRE(item.ptr == 0);
    //   }
    // }
    // auto iter = tree.GetLeafsIterator();
    // std::size_t count = (*iter)->GetChildCount();
    // for (std::size_t i = 0; i != count; ++i) {
    //   for (const auto& ptr : (*iter)->GetData()) {
    //     REQUIRE(ptr.start != 0);
    //     REQUIRE(ptr.end != 0);
    //     REQUIRE(ptr.ptr == 0);
    //   }
    //   ++iter;
    // }
  };
}

TEST_CASE("Basic node tests", "[TimeTreeNode]") {
  auto leafNode = TimeTreeNode<2>(true, 0, 10, 0);
  REQUIRE(leafNode.GetNodeStart() == 0);
  REQUIRE(leafNode.GetNodeEnd() == 10);
  REQUIRE(leafNode.Insert(12, 11, 0) == tl::unexpected(Errors_e::INVALID_TIME_RANGE));
  // REQUIRE(leafNode.Insert(9, 11, 0) == tl::unexpected(Errors_e::INVALID_TIME_RANGE));

  auto treeNode = TimeTreeNode<2>(false, 0, 10, 0);
  REQUIRE(treeNode.Insert(11, 12, 0) == tl::unexpected(Errors_e::NON_LEAF_PTR_INSERT));
  REQUIRE(treeNode.GetNodeStart() == 0);
  REQUIRE(treeNode.GetNodeEnd() == 10);
}

TEST_CASE("Query tests", "[TimeTree querying]") {
  SECTION("Simple queries, ary: 2") {
    TimeTree<2> tree;
    for (int i = 1; i <= 16; ++i) {
      tree.Insert(i, i, 0);
    }

    tree.PrintTree();

    REQUIRE(tree.Query(2, 1) == tl::unexpected(Errors_e::INVALID_TIME_RANGE));
    REQUIRE(tree.Query(20, 21) == tl::unexpected(Errors_e::RANGE_NOT_IN_DB));

    auto qRes = tree.Query(1, 1);
    REQUIRE(qRes.has_value() == true);
    std::vector<TimeRange_t> q = qRes.value();

    REQUIRE(q.size() == 1);

    qRes = tree.Query(1, 16);
    REQUIRE(qRes.has_value() == true);
    REQUIRE(qRes->size() == 16);
  };
  SECTION("Simple queries, ary: 4") {
    TimeTree<4> tree;
    for (int i = 1; i <= 16; ++i) {
      tree.Insert(i, i, 0);
    }

    tree.PrintTree();

    REQUIRE(tree.Query(2, 1) == tl::unexpected(Errors_e::INVALID_TIME_RANGE));
    REQUIRE(tree.Query(20, 21) == tl::unexpected(Errors_e::RANGE_NOT_IN_DB));

    auto qRes = tree.Query(1, 1);
    REQUIRE(qRes.has_value() == true);
    std::vector<TimeRange_t> q = qRes.value();

    REQUIRE(q.size() == 1);

    qRes = tree.Query(2, 13);
    REQUIRE(qRes.has_value() == true);
    REQUIRE(qRes->size() == 12);
  };
  SECTION("Sparse queries") {
    TimeTree<4> tree;
    for (int i = 10; i <= 100; i += 10) {
      tree.Insert(i, i + 5, 0);
    }

    tree.PrintTree();

    auto qRes = tree.Query(50, 60);
    REQUIRE(qRes.has_value() == true);
    REQUIRE(qRes->size() == 2);

    qRes = tree.Query(45, 55);
    REQUIRE(qRes != tl::unexpected(Errors_e::RANGE_NOT_IN_DB));
    REQUIRE(qRes.has_value() == true);
    REQUIRE(qRes->size() == 2);
  };
  SECTION("Big queries") {
    TimeTree<64> tree;
    for (int i = 1; i <= 100000; ++i) {
      tree.Insert(i, i, 0);
    }
    // auto i = GENERATE(random(1, 100000));
    // auto j = GENERATE(random(i, 100000 - i));

    // 10 samples, generated using fair dicerolls
    for (auto [a, b] : std::vector<std::pair<std::size_t, std::size_t>>{
             {6347, 84682},
             {20029, 36875},
             {7154, 39037},
             {48796, 49270},
             {25895, 49156},
             {26374, 48889},
             {39678, 43239},
             {48409, 51404},
             {8831, 72340},
             {10436, 65401}}) {
      auto res = tree.Query(a, b);
      REQUIRE(res.has_value());
      REQUIRE(res->size() == (b - a) + 1);
    }
    // res = tree.Query(6347, 84682);
    // REQUIRE(res.has_value());
  };
}

TEST_CASE("TimeTree leaf iterator", "[TimeTree]") {
  TimeTree<4> tree;
  for (int i = 1; i <= 32; ++i) {
    tree.Insert(i, i, 0);
  }

  std::deque<std::deque<TimeTreeNode<4>*>>& nodes = tree.Data();
  nodes.at(0).erase(nodes.at(0).begin(), nodes.at(0).begin() + 4);
  nodes.at(1).front()->ConvertToLeaf();

  for (TimeTreeNode<4>& node : tree) {
    fmt::print("{} -> {}\n", node.GetNodeStart(), node.GetNodeEnd());
  }

  // auto it = tree.begin();
  // for (int i = 0; i < 4; ++i) {
  //   // it->ConvertToLeaf();
  //   it = std::next(it);
  // }
}

TEST_CASE("TimeTree aggregation", "[TimeTree]") {
  TimeTree<4> tree;
  for (int i = 1; i <= 32; ++i) {
    tree.Insert(i, i, 0);
  }

  tree.PrintTree();

  std::vector<TimeRange_t> res;
  tree.Aggregate(5, res);
  REQUIRE(res.size() == 4);

  fmt::print("removed size: {}\n", res.size());
  for (TimeTreeNode<4> node : tree) {
    fmt::print("{} -> {}\n", node.GetNodeStart(), node.GetNodeEnd());
  }

  res.clear();
  tree.Aggregate(16, res);
  fmt::print("removed size: {}\n", res.size());
  REQUIRE(res.size() == 12);
  tree.PrintTree();

  for (TimeTreeNode<4>& node : tree) {
    fmt::print("{} -> {}\n", node.GetNodeStart(), node.GetNodeEnd());
  }

  res.clear();
  tree.Aggregate(16, res);
  fmt::print("removed size: {}\n", res.size());
  REQUIRE(res.size() == 4);
  tree.PrintTree();

  for (TimeTreeNode<4>& node : tree) {
    fmt::print("{} -> {}\n", node.GetNodeStart(), node.GetNodeEnd());
  }

  res.clear();
  tree.Aggregate(21, res);
  fmt::print("removed size: {}\n", res.size());
  REQUIRE(res.size() == 4);
  tree.PrintTree();

  for (TimeTreeNode<4>& node : tree) {
    fmt::print("{} -> {}\n", node.GetNodeStart(), node.GetNodeEnd());
  }
}

// TEST_CASE("TimeTree benchmark", "[TimeTree]") {
//   BENCHMARK("100 Inserts, arity: 2") {
//     TimeTree<2> tree;
//     for (int i = 0; i < 100; ++i) {
//       tree.Insert(i, i, 0);
//     }
//   };

//   BENCHMARK("100k Inserts, arity: 2") {
//     TimeTree<2> tree;
//     for (int i = 0; i < 100000; ++i) {
//       tree.Insert(i, i, 0);
//     }
//   };

//   BENCHMARK("100 Inserts, arity: 64") {
//     TimeTree<64> tree;
//     for (int i = 0; i < 100; ++i) {
//       tree.Insert(i, i, 0);
//     }
//   };

//   BENCHMARK("100k Inserts, arity: 64") {
//     TimeTree<64> tree;
//     for (int i = 0; i < 100000; ++i) {
//       tree.Insert(i, i, 0);
//     }
//   };
// }
