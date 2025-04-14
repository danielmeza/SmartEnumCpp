cmake_minimum_required(VERSION 3.10)
project(SmartEnumCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include project headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Build the test executable
enable_testing()

# Assume GoogleTest is added as a subdirectory or installed on your system.
# Here we assume it’s fetched via CMake's FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(SmartEnumTests tests/test_SmartEnum.cpp)
target_link_libraries(SmartEnumTests gtest_main)

include(GoogleTest)
gtest_discover_tests(SmartEnumTests)
