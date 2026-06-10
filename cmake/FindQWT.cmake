# FindQWT.cmake
# Finds the QWT library
#
# This will define:
#   QWT_FOUND        - True if QWT was found
#   QWT_INCLUDE_DIRS - Include directories for QWT
#   QWT_LIBRARIES    - Libraries to link against
#
# Usage:
#   set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
#   find_package(QWT)
#   Or pass -DQWT_ROOT=/path/to/qwt

find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    PATHS
        ${QWT_ROOT}/include
        /usr/local/qwt-6.1.3/include
        /usr/local/qwt-6.2.0/include
        /usr/local/include/qwt
        /usr/include/qwt6
        /usr/include/qwt
)

find_library(QWT_LIBRARY
    NAMES qwt qwt6 qwt-qt5 qwt6-qt5
    PATHS
        ${QWT_ROOT}/lib
        /usr/local/qwt-6.1.3/lib
        /usr/local/qwt-6.2.0/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QWT DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR)

if(QWT_FOUND)
    set(QWT_INCLUDE_DIRS "${QWT_INCLUDE_DIR}")
    set(QWT_LIBRARIES "${QWT_LIBRARY}")
    message(STATUS "QWT found: ${QWT_LIBRARY}")
    message(STATUS "QWT include: ${QWT_INCLUDE_DIR}")
else()
    message(WARNING "QWT not found. Set -DQWT_ROOT=<path> or install QWT to build QWT-dependent demos.")
endif()

mark_as_advanced(QWT_INCLUDE_DIR QWT_LIBRARY)
