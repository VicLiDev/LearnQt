# FindQWT.cmake - Find QWT library
#
# This module defines:
#   QWT_FOUND       - True if QWT is found
#   QWT_INCLUDE_DIRS - The QWT include directories
#   QWT_LIBRARIES   - The libraries needed to use QWT

find_path(QWT_INCLUDE_DIR
    NAMES qwt.h
    PATHS /usr/include/qwt /usr/local/include/qwt
)

find_library(QWT_LIBRARY
    NAMES qwt-qt5 qwt qwt6
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QWT DEFAULT_MSG QWT_LIBRARY QWT_INCLUDE_DIR)

if(QWT_FOUND)
    set(QWT_INCLUDE_DIRS "${QWT_INCLUDE_DIR}")
    set(QWT_LIBRARIES "${QWT_LIBRARY}")
endif()

mark_as_advanced(QWT_INCLUDE_DIR QWT_LIBRARY)
