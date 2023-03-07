add_compile_options(-std=c++20 -O0 -g -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wunused -Wpedantic)
set(testSources
  main.cpp
  )

find_package(Catch2 REQUIRED)
add_executable(tests ${testSources})
target_link_libraries(tests PUBLIC
  TimeTree
  Catch2::Catch2
  )

include(CTest)
include(Catch)
catch_discover_tests(tests)
