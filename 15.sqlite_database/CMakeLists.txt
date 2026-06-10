cmake_minimum_required(VERSION 3.16)
project(sqlite_database LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/databaseencap.cpp
    src/databaseuserwork.cpp
    src/databasewidget.cpp
)
set(HEADERS
    src/mainwindow.h
    src/databaseencap.h
    src/databaseuserwork.h
    src/databasewidget.h
)
set(FORMS
    src/mainwindow.ui
    src/databasewidget.ui
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Sql)
