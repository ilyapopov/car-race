cmake_minimum_required(VERSION 3.5)
project(car-race CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTING OFF)
add_subdirectory(extern/abseil-cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_executable(cars cars.cpp)
target_compile_options(cars PRIVATE -Wall -Wextra)
target_link_libraries(cars PRIVATE absl::flat_hash_map Threads::Threads)
target_include_directories(cars PRIVATE extern/hopscotch-map/include)
target_include_directories(cars PRIVATE extern/robin-map/include)
target_include_directories(cars PRIVATE extern/skarupke)
target_include_directories(cars PRIVATE extern/robin-hood-hashing/src/include)
