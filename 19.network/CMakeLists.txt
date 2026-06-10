cmake_minimum_required(VERSION 3.16)
project(network LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
)
set(HEADERS
    src/mainwindow.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)
