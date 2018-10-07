cmake_minimum_required(VERSION 3.5)
project(cars CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(extern/abseil-cpp)

add_executable(cars cars.cpp)
target_compile_options(cars PRIVATE -Wall -Wextra)
target_link_libraries(cars PRIVATE absl::container absl::hash)
target_include_directories(cars PRIVATE extern/hopscotch-map/include)
target_include_directories(cars PRIVATE extern/robin-map/include)
target_include_directories(cars PRIVATE extern/skarupke)
