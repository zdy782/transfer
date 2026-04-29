cmake_minimum_required(VERSION 2.8)

project(sodium_bench)
file(GLOB_RECURSE libraries "./src/*.hpp")
file(GLOB_RECURSE sources "./src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Ofast -g3 -std=c++14")

# Use locally built libsodium instead of system-installed version
set(LIBSODIUM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../libsodium")
set(LIBSODIUM_INCLUDE "${LIBSODIUM_SRC}/src/libsodium/include")
set(LIBSODIUM_LIB "${LIBSODIUM_SRC}/src/libsodium/.libs")

add_executable(${PROJECT_NAME} ${sources} ${libraries})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBSODIUM_INCLUDE})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBSODIUM_LIB})
target_link_libraries(sodium_bench sodium benchmark pthread)
