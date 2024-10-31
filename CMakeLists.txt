cmake_minimum_required(VERSION 3.16)

project(pomodoro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

qt_add_executable(pomodoro
    main.cpp
)

target_link_libraries(pomodoro PRIVATE Qt6::Core)
