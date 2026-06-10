cmake_minimum_required(VERSION 3.16)
project(custom_plot_advanced LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Use system Qt5.15 (QWT library is built against Qt5.15)
set(Qt5_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5" CACHE PATH "" FORCE)

find_package(Qt5 REQUIRED COMPONENTS Widgets Xml PrintSupport Svg OpenGL)

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
    src/dialog.cpp
    src/irregulardialog.cpp
    src/qcustomplot.cpp
)
set(HEADERS
    src/mainwindow.h
    src/dialog.h
    src/irregulardialog.h
    src/qcustomplot.h
)
set(FORMS
    src/mainwindow.ui
    src/dialog.ui
    src/irregulardialog.ui
)
set(RESOURCES
    resources/myresource.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src ${QWT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Xml Qt5::PrintSupport Qt5::Svg Qt5::OpenGL ${QWT_LIBRARIES})
