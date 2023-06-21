#include <cstdint>
#define ANKERL_NANOBENCH_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "src/TimeTree.hpp"

#include <doctest.h>
#include <list>
#include <nanobench.h>

#define GEN_INSERT_TEST(SIZE)                                                                                          \
  SUBCASE("Arity of " #SIZE) {                                                                                         \
    TimeTree<SIZE> tree;                                                                                               \
    uint64_t start = 1;                                                                                                \
    uint64_t end = 1;                                                                                                  \
    ankerl::nanobench::Bench().minEpochIterations(1000000).performanceCounters(true).run(                              \
        "Arity: " #SIZE " insertion",                                                                                  \
        [&] {                                                                                                          \
          tree.Insert(start, end, 0);                                                                                  \
          ++start;                                                                                                     \
          ++end;                                                                                                       \
        });                                                                                                            \
  };

#define GEN_QUERY_TEST(SIZE)                                                                                           \
  SUBCASE("Arity of " #SIZE) {                                                                                         \
    std::ofstream output("QueryArity" #SIZE);                                                                          \
    ankerl::nanobench::Bench bench;                                                                                    \
    bench.title("Arity of " #SIZE).unit("Query");                                                                      \
    bench.performanceCounters(true);                                                                                   \
    std::list<int> tmp;                                                                                                \
    TimeTree<SIZE> tree;                                                                                               \
    uint64_t start = 1;                                                                                                \
    uint64_t end = 1;                                                                                                  \
    while (start != 1'000'000) {                                                                                       \
      if (start % 100 == 0) {                                                                                          \
        for (int i = 0; i < 1'000; ++i) {                                                                              \
          tmp.push_back(i);                                                                                            \
        }                                                                                                              \
      }                                                                                                                \
      tree.Insert(start, end, 0);                                                                                      \
      ++start;                                                                                                         \
      ++end;                                                                                                           \
    }                                                                                                                  \
    bench.run("Arity: " #SIZE " lookup 10", [&] {                                                                      \
      auto res = tree.Query(10, 20);                                                                                   \
      ankerl::nanobench::doNotOptimizeAway(res);                                                                       \
    });                                                                                                                \
    bench.run("Arity: " #SIZE " lookup 1k", [&] {                                                                      \
      auto res = tree.Query(1'000, 2'000);                                                                             \
      ankerl::nanobench::doNotOptimizeAway(res);                                                                       \
    });                                                                                                                \
    bench.run("Arity: " #SIZE " lookup 10k", [&] {                                                                     \
      auto res = tree.Query(10'000, 20'000);                                                                           \
      ankerl::nanobench::doNotOptimizeAway(res);                                                                       \
    });                                                                                                                \
    bench.run("Arity: " #SIZE " lookup 100k", [&] {                                                                    \
      auto res = tree.Query(100'000, 200'000);                                                                         \
      ankerl::nanobench::doNotOptimizeAway(res);                                                                       \
    });                                                                                                                \
    bench                                                                                                              \
        .run(                                                                                                          \
            "Arity: " #SIZE " lookup 1M",                                                                              \
            [&] {                                                                                                      \
              auto res = tree.Query(1, 1'000'000);                                                                     \
              ankerl::nanobench::doNotOptimizeAway(res);                                                               \
            })                                                                                                         \
        .render(ankerl::nanobench::templates::csv(), output);                                                          \
  };

TEST_CASE("Insertion Bench") {
  GEN_INSERT_TEST(8);
  GEN_INSERT_TEST(16);
  GEN_INSERT_TEST(32);
  GEN_INSERT_TEST(64);
  GEN_INSERT_TEST(128);
  GEN_INSERT_TEST(160);
  GEN_INSERT_TEST(180);
  GEN_INSERT_TEST(200);
  GEN_INSERT_TEST(220);
  GEN_INSERT_TEST(240);
  GEN_INSERT_TEST(256);
}

TEST_CASE("Lookup Bench") {
  GEN_QUERY_TEST(8);
  GEN_QUERY_TEST(16);
  GEN_QUERY_TEST(32);
  GEN_QUERY_TEST(64);
  GEN_QUERY_TEST(128);
  GEN_QUERY_TEST(160);
  GEN_QUERY_TEST(180);
  GEN_QUERY_TEST(200);
  GEN_QUERY_TEST(220);
  GEN_QUERY_TEST(240);
  GEN_QUERY_TEST(256);
}