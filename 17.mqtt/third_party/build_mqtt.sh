#!/bin/bash
# build_mqtt.sh - 从源码编译 Qt MQTT 库到 third_party 目录
# 需要: qtbase5-dev qtbase5-private-dev

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
INSTALL_DIR="${SCRIPT_DIR}/qt-mqtt"
BUILD_DIR="/tmp/qtmqtt-build"
SRC_DIR="/tmp/qtmqtt-src"

echo "=== 构建 Qt MQTT ==="

# 1. 克隆源码（如果不存在）
if [ ! -d "${SRC_DIR}" ]; then
    echo "克隆 Qt MQTT v5.15.2 源码..."
    git clone --depth 1 --branch v5.15.2 https://code.qt.io/qt/qtmqtt.git "${SRC_DIR}"
fi

# 2. 准备构建目录
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}/include/QtMqtt"
cp "${SRC_DIR}/src/mqtt/"*.h "${BUILD_DIR}/include/QtMqtt/"

# 3. 创建 PascalCase 转发头文件
for name in QMqttAuthenticationProperties QMqttClient QMqttConnectionProperties \
             QMqttMessage QMqttPublishProperties QMqttSubscription \
             QMqttSubscriptionProperties QMqttTopicFilter QMqttTopicName \
             QMqttType QMqttGlobal; do
    lower=$(echo "$name" | sed 's/Q/q/; s/\([A-Z]\)/_\l\1/g; s/^_//')
    echo "#include \"${lower}.h\"" > "${BUILD_DIR}/include/QtMqtt/${name}"
done

# 4. 创建 .pro 文件并编译
cd "${BUILD_DIR}"
cat > qtmqtt_standalone.pro << 'PROEOF'
TEMPLATE = lib
CONFIG += c++11 shared qt
QT = core network
TARGET = Qt5Mqtt

INCLUDEPATH += include \
    /usr/include/x86_64-linux-gnu/qt5 \
    /usr/include/x86_64-linux-gnu/qt5/QtCore/5.15.13 \
    /usr/include/x86_64-linux-gnu/qt5/QtCore/5.15.13/QtCore \
    /usr/include/x86_64-linux-gnu/qt5/QtNetwork/5.15.13 \
    /usr/include/x86_64-linux-gnu/qt5/QtNetwork/5.15.13/QtNetwork

HEADERS += include/QtMqtt/qmqttclient.h \
    include/QtMqtt/qmqttconnection_p.h \
    include/QtMqtt/qmqttmessage.h \
    include/QtMqtt/qmqttsubscription.h \
    include/QtMqtt/qmqttglobal.h

SOURCES += \
    /tmp/qtmqtt-src/src/mqtt/qmqttauthenticationproperties.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttclient.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttconnection.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttconnectionproperties.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttcontrolpacket.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttmessage.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttpublishproperties.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttsubscription.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqttsubscriptionproperties.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqtttopicfilter.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqtttopicname.cpp \
    /tmp/qtmqtt-src/src/mqtt/qmqtttype.cpp
PROEOF

/usr/lib/qt5/bin/qmake qtmqtt_standalone.pro
make -j$(nproc)

# 5. 安装到 third_party
mkdir -p "${INSTALL_DIR}/lib" "${INSTALL_DIR}/include/QtMqtt" \
         "${INSTALL_DIR}/lib/cmake/Qt5Mqtt"
cp libQt5Mqtt.so* "${INSTALL_DIR}/lib/"
cp include/QtMqtt/*.h "${INSTALL_DIR}/include/QtMqtt/"

cat > "${INSTALL_DIR}/lib/cmake/Qt5Mqtt/Qt5MqttConfig.cmake" << CMAKEEOF
set(Qt5Mqtt_FOUND TRUE)
set(Qt5Mqtt_VERSION "5.15.2")
set(Qt5Mqtt_INCLUDE_DIRS "\${CMAKE_CURRENT_LIST_DIR}/../../../include")
set(Qt5Mqtt_LIBRARIES Qt5Mqtt)

add_library(Qt5Mqtt::Mqtt SHARED IMPORTED)
set_target_properties(Qt5Mqtt::Mqtt PROPERTIES
    IMPORTED_LOCATION "\${CMAKE_CURRENT_LIST_DIR}/../../libQt5Mqtt.so.1.0.0"
    INTERFACE_INCLUDE_DIRECTORIES "\${CMAKE_CURRENT_LIST_DIR}/../../../include"
    IMPORTED_LINK_INTERFACE_LIBRARIES "Qt5::Network;Qt5::Core"
)
if(NOT TARGET Qt5::Mqtt)
    add_library(Qt5::Mqtt ALIAS Qt5Mqtt::Mqtt)
endif()
CMAKEEOF

echo "=== 构建完成: ${INSTALL_DIR} ==="
