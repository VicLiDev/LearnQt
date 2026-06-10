cmake_minimum_required(VERSION 3.16)
project(mqtt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# ============================================================
# Qt5 查找：优先使用系统 Qt5.15（MQTT 模块需要）
# 其他 demo 使用 Qt5.13，但 Qt MQTT 模块只在 Qt5.15 上编译成功
# ============================================================
list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt5")

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

# ============================================================
# Qt5Mqtt：从项目本地 third_party 目录加载
# 该库从 Qt 源码编译安装到此位置
# ============================================================
set(QT_MQTT_ROOT "${CMAKE_SOURCE_DIR}/third_party/qt-mqtt")
find_package(Qt5Mqtt QUIET
    NO_DEFAULT_PATH
    HINTS "${QT_MQTT_ROOT}/lib/cmake"
)

if(NOT Qt5Mqtt_FOUND)
    message(WARNING
        "Qt5Mqtt 未找到。\n"
        "请先编译安装 Qt MQTT 模块到 ${QT_MQT_ROOT}/\n"
        "参考 README.md 中的安装说明。"
    )
endif()

# ============================================================
# 源文件
# ============================================================
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mqtt_client_wrapper.cpp
    src/tab_publish_subscribe.cpp
    src/tab_qos_level.cpp
    src/tab_topic_wildcard.cpp
    src/tab_lwt.cpp
    src/tab_retained.cpp
    src/tab_auth_tls.cpp
    src/tab_request_response.cpp
    src/tab_batch_publish.cpp
    src/tab_message_monitor.cpp
)
set(HEADERS
    src/mainwindow.h
    src/mqtt_client_wrapper.h
    src/tab_publish_subscribe.h
    src/tab_qos_level.h
    src/tab_topic_wildcard.h
    src/tab_lwt.h
    src/tab_retained.h
    src/tab_auth_tls.h
    src/tab_request_response.h
    src/tab_batch_publish.h
    src/tab_message_monitor.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    "${QT_MQTT_ROOT}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Network
    Qt5::Mqtt
)

# 设置 RPATH，确保运行时能找到本地 Qt5Mqtt 库
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "${QT_MQTT_ROOT}/lib"
    INSTALL_RPATH "$ORIGIN/../third_party/qt-mqtt/lib"
    INSTALL_RPATH_USE_LINK_RPATH FALSE
)
