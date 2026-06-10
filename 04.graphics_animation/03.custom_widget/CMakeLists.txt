cmake_minimum_required(VERSION 3.16)
project(custom_widget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

# ============================================================
# 源文件
# ============================================================
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/circle_button.cpp
    src/gauge_widget.cpp
    src/star_delegate.cpp
    src/file_path_selector.cpp
)
set(HEADERS
    src/mainwindow.h
    src/circle_button.h
    src/gauge_widget.h
    src/star_delegate.h
    src/file_path_selector.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
