cmake_minimum_required(VERSION 3.16)
project(ipc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)
