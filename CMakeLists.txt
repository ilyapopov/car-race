cmake_minimum_required(VERSION 3.5)
project(car-race CXX)

option(BUILD_TESTING OFF)
add_subdirectory(extern/abseil-cpp)

add_executable(cars cars.cpp)
target_compile_options(cars PRIVATE -Wall -Wextra)
target_link_libraries(cars PRIVATE absl::flat_hash_map)
target_compile_features(cars PRIVATE cxx_std_17)
target_include_directories(cars PRIVATE extern/hopscotch-map/include)
target_include_directories(cars PRIVATE extern/robin-map/include)
target_include_directories(cars PRIVATE extern/skarupke)
target_include_directories(cars PRIVATE extern/robin-hood-hashing/src/include)
