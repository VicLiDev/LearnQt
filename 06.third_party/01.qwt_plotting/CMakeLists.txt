cmake_minimum_required(VERSION 3.16)
project(qwt_plotting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Use system Qt5.15 (QWT library is built against Qt5.15)
set(Qt5_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5" CACHE PATH "" FORCE)

find_path(MYSQL_INCLUDE_DIR NAMES mysql/mysql.h PATH_SUFFIXES include)
if(NOT MYSQL_INCLUDE_DIR)
    message(WARNING
        "MySQL client dev headers not found!\n"
        "  Install with: sudo apt install libmysqlclient-dev"
    )
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
find_package(QWT)

if(NOT QWT_FOUND)
    message(FATAL_ERROR
        "QWT library not found!\n"
        "  Install with: sudo apt install libqwt-qt5-dev"
    )
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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Sql ${QWT_LIBRARIES})
