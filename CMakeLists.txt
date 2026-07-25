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
)

# Anything that links against sentinel_hardware automatically gets this
# include path, so #include "photoeye.hpp" resolves correctly.
target_include_directories(sentinel_hardware PUBLIC controller/include)

# --- The test-harness executable ---
add_executable(sentinel_sim controller/src/main.cpp)
target_link_libraries(sentinel_sim PRIVATE sentinel_hardware)
