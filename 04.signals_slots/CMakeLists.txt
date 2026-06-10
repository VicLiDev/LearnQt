cmake_minimum_required(VERSION 3.16)
project(signals_slots LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES src/main.cpp src/mainwindow.cpp src/counter.cpp)
set(HEADERS src/mainwindow.h src/counter.h)
set(FORMS src/mainwindow.ui)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
