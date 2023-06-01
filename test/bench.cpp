#include <cstdint>
#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <nanobench.h>
#include <list>

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
  // SUBCASE("true loop") {
  //   TimeTree<64> tree;
  //   uint64_t start = 1;
  //   uint64_t end = 1;
  //   while(true) {
  //     tree.Insert(start, end, 0);
  //     ++start;
  //     ++end;
  //   }
  // };
}

TEST_CASE("Lookup Bench") {
  SUBCASE("Arity of 2") {
    ankerl::nanobench::Bench b;
    b.title("Arity of 2")
      .unit("Query")
      .warmup(50);
    b.performanceCounters(true);

    std::list<int> tmp; // Used for spraying the heap
    TimeTree<2> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    while(start != 1'000'000) {
      if(start % 100 == 0) {
        for(int i = 0; i < 1'000; ++i) {
          tmp.push_back(i);
        }
      }
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    }
    
    SUBCASE("Lookup 10") {
      b.minEpochIterations(800000).run("Arity: 2 lookup 10", [&] {
        auto res = tree.Query(10, 20);
        ankerl::nanobench::doNotOptimizeAway(res);
        // assert(res.has_value());
      });
    };
    SUBCASE("Lookup 1k") {
      b.run("Arity: 2 lookup 1k", [&] {
        auto res = tree.Query(1'000, 2'000);
        ankerl::nanobench::doNotOptimizeAway(res);
        // assert(res.has_value());
      });
    };
    SUBCASE("Lookup 10k") {
      b.run("Arity: 2 lookup 10k", [&] {
        auto res = tree.Query(10'000, 20'000);
        ankerl::nanobench::doNotOptimizeAway(res);
        // assert(res.has_value());
      });
    };
    SUBCASE("Lookup 100k") {
      b.minEpochIterations(20).run("Arity: 2 lookup 100k", [&] {
        auto res = tree.Query(100'000, 200'000);
        ankerl::nanobench::doNotOptimizeAway(res);
        // assert(res.has_value());
        // assert(res->size() == 100'000);
      });
    };
    SUBCASE("Lookup 1M") {
      b.run("Arity: 2 lookup 1M", [&] {
        auto res = tree.Query(1, 1'000'000);
        ankerl::nanobench::doNotOptimizeAway(res);
        // assert(res.has_value());
      });
    };
  };
  SUBCASE("Arity of 64") {
    ankerl::nanobench::Bench b;
    b.title("Arity of 64")
      .unit("Query")
      .warmup(50);
    b.performanceCounters(true);

    std::list<int> tmp; // Used for spraying the heap
    TimeTree<64> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    while(start != 1'000'000) {
      if(start % 100 == 0) {
        for(int i = 0; i < 1'000; ++i) {
          tmp.push_back(i);
        }
      }
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    }
    
    SUBCASE("Lookup 10") {
      b.minEpochIterations(800000).run("Arity: 64 lookup 10", [&] {
        auto res = tree.Query(10, 20);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 1k") {
      b.run("Arity: 64 lookup 1k", [&] {
        auto res = tree.Query(1'000, 2'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 10k") {
      b.run("Arity: 64 lookup 10k", [&] {
        auto res = tree.Query(10'000, 20'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 100k") {
      b.minEpochIterations(20).run("Arity: 64 lookup 100k", [&] {
        auto res = tree.Query(100'000, 200'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 1M") {
      b.run("Arity: 64 lookup 1M", [&] {
        auto res = tree.Query(1, 1'000'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
  };
  SUBCASE("Arity of 256") {
    ankerl::nanobench::Bench b;
    b.title("Arity of 256")
      .unit("Query")
      .warmup(50);
    b.performanceCounters(true);

    std::list<int> tmp; // Used for spraying the heap
    TimeTree<64> tree;
    uint64_t start = 1;
    uint64_t end = 1;
    while(start != 1'000'000) {
      if(start % 100 == 0) {
        for(int i = 0; i < 1'000; ++i) {
          tmp.push_back(i);
        }
      }
      tree.Insert(start, end, 0);
      ++start;
      ++end;
    }
    
    SUBCASE("Lookup 10") {
      b.minEpochIterations(800000).run("Arity: 256 lookup 10", [&] {
        auto res = tree.Query(10, 20);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 1k") {
      b.run("Arity: 256 lookup 1k", [&] {
        auto res = tree.Query(1'000, 2'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 10k") {
      b.run("Arity: 256 lookup 10k", [&] {
        auto res = tree.Query(10'000, 20'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 100k") {
      b.minEpochIterations(20).run("Arity: 256 lookup 100k", [&] {
        auto res = tree.Query(100'000, 200'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
    SUBCASE("Lookup 1M") {
      b.run("Arity: 256 lookup 1M", [&] {
        auto res = tree.Query(1, 1'000'000);
        ankerl::nanobench::doNotOptimizeAway(res);
      });
    };
  };
}