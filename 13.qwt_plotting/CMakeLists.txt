cmake_minimum_required(VERSION 3.16)
project(qwt_plotting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
find_package(QWT)

if(NOT QWT_FOUND)
    message(FATAL_ERROR "QWT not found. Set -DQWT_ROOT=/path/to/qwt or install QWT.")
endif()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/myqwtscaledraw.cpp
)
set(HEADERS
    src/mainwindow.h
)
set(FORMS
    src/mainwindow.ui
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})
target_include_directories(${PROJECT_NAME} PRIVATE ${QWT_INCLUDE_DIRS} src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets ${QWT_LIBRARIES})
