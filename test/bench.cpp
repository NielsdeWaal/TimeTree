#include <cstdint>
#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nanobench.h>

#include "src/TimeTree.hpp"

// int main() {
//     int y = 0;
//     std::atomic<int> x(0);
//     ankerl::nanobench::Bench().run("compare_exchange_strong", [&] {
//         x.compare_exchange_strong(y, 0);
//     });
// }

TEST_CASE("Insertion Bench") {
  SUBCASE("Arity of 2") {
    TimeTree<2> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    ankerl::nanobench::Bench().minEpochIterations(300).performanceCounters(true).run("Arity: 2 insertion", [&] {
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    });
  };
  SUBCASE("Arity of 64") {
    TimeTree<64> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    ankerl::nanobench::Bench().minEpochIterations(300).performanceCounters(true).run("Arity: 64 insertion", [&] {
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    });
  };
  SUBCASE("Arity of 64 (long)") {
    TimeTree<64> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    ankerl::nanobench::Bench().minEpochIterations(800000).performanceCounters(true).run("Arity: 64 insertion", [&] {
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    });
  };
  SUBCASE("true loop") {
    TimeTree<64> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    while(true) {
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    }
  };
}