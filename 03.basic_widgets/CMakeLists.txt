cmake_minimum_required(VERSION 3.16)
project(basic_widgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES src/main.cpp src/mainwindow.cpp)
set(HEADERS src/mainwindow.h)
set(FORMS src/mainwindow.ui)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
