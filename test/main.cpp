#include <cstdint>
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
    REQUIRE((*std::next(iter))->GetChildCount() == 1);
    REQUIRE((*std::next(iter))->GetLink() == start);

    uint64_t tester = 1;
    for (TimeTreeNode<2>* node : tree) {
      REQUIRE(node->GetNodeStart() != 0);
      REQUIRE(node->GetNodeEnd() != 0);
      const std::span<TimeRange_t> range = node->GetData();
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
    for (TimeTreeNode<8>* node : tree) {
      REQUIRE(node->GetNodeStart() != 0);
      REQUIRE(node->GetNodeEnd() != 0);
      const std::span<TimeRange_t> range = node->GetData();
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
