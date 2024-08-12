cmake_minimum_required(VERSION 3.24)
project(Advent_of_Code_2023)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0")

#include(GNUInstallDirs)
message(${CMAKE_CXX_COMPILER_VERSION})
message(${CMAKE_CXX_COMPILER})


add_library(help_functions INTERFACE)
target_sources(help_functions INTERFACE include/help_func.hpp)
target_include_directories(help_functions INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set_source_files_properties(src/day1.cpp PROPERTIES COMPILE_OPTIONS "-g;-fsanitize=leak;-O3;")
add_executable(day1.out src/day1.cpp)
target_link_libraries(day1.out PUBLIC help_functions)
target_link_libraries(day1.out PRIVATE Threads::Threads)
target_include_directories(day1.out PRIVATE data)

# target_link_libraries(main.out PRIVATE treelib)

# enable_testing()
# find_package(GTest REQUIRED)
# add_subdirectory(tests)
