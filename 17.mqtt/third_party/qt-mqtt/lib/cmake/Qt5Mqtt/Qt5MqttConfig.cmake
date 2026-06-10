# Qt5Mqtt CMake 配置（从源码编译安装到项目本地）
set(Qt5Mqtt_FOUND TRUE)
set(Qt5Mqtt_VERSION "5.15.2")
set(Qt5Mqtt_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../include")
set(Qt5Mqtt_LIBRARIES Qt5Mqtt)

add_library(Qt5Mqtt::Mqtt SHARED IMPORTED)
set_target_properties(Qt5Mqtt::Mqtt PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../libQt5Mqtt.so.1.0.0"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../../include"
    IMPORTED_LINK_INTERFACE_LIBRARIES "Qt5::Network;Qt5::Core"
)

if(NOT TARGET Qt5::Mqtt)
    add_library(Qt5::Mqtt ALIAS Qt5Mqtt::Mqtt)
endif()
