# CMakeList.txt : CMake project for thread design, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.20)
project(Thread_make LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Thread_make
    src/app/main.cpp
    src/app/command_input.cpp
    src/core/controller.cpp
    src/core/alarm_manager.cpp
    src/core/logger.cpp
    src/drivers/door_sensor_mock.cpp
    src/drivers/access_verifier_mock.cpp
 )

target_include_directories(Thread_make PRIVATE src)

# TODO: Add tests and install targets if needed.
