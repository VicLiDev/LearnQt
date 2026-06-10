cmake_minimum_required(VERSION 3.14)
project(timer_thread LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/worker.h
    src/worker.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
