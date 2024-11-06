cmake_minimum_required(VERSION 3.16)

project(pomodoro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/timer.cpp
)

qt_add_executable(pomodoro
    ${SOURCES}
)

target_link_libraries(pomodoro PRIVATE Qt6::Core)
