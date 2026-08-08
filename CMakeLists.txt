cmake_minimum_required(VERSION 3.20)
project(sentinel CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- The hardware simulation library ---
# Everything except main.cpp gets compiled into a "library" — a reusable
# bundle of compiled code that both the main program and (later) the test
# suite can link against, without recompiling it twice.
add_library(sentinel_hardware
    controller/src/photoeye.cpp
    controller/src/encoder.cpp
    controller/src/proximity_sensor.cpp
    controller/src/conveyor_motor.cpp
    controller/src/diverter_cylinder.cpp
    controller/src/conveyor_line.cpp
    controller/src/watchdog.cpp
    controller/src/controller.cpp
    controller/src/modbus_server.cpp
)

# Anything that links against sentinel_hardware automatically gets this
# include path, so #include "photoeye.hpp" resolves correctly.
target_include_directories(sentinel_hardware PUBLIC controller/include)

# --- The test-harness executable ---
add_executable(sentinel_sim controller/src/main.cpp)
target_link_libraries(sentinel_sim PRIVATE sentinel_hardware)

# --- Testing ---
# Pull in GoogleTest automatically via FetchContent, so nobody who clones
# this repo needs to install it separately — CMake downloads and builds
# it as part of our own build.
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
# On Windows, prevents overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Turns on CTest, so `ctest` can discover and run the tests below.
enable_testing()

add_executable(sentinel_tests
    controller/tests/test_hardware.cpp
    controller/tests/test_controller.cpp
)
target_link_libraries(sentinel_tests PRIVATE sentinel_hardware GTest::gtest_main)

# Scans sentinel_tests and registers each individual TEST(...) as its own
# CTest entry, so `ctest --output-on-failure` reports failures test-by-test
# instead of as one opaque pass/fail blob.
include(GoogleTest)
gtest_discover_tests(sentinel_tests)
