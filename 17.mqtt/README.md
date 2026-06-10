# 17.mqtt — Qt MQTT 综合学习 Demo

## 概述

学习使用 Qt MQTT 模块（QMqttClient）实现物联网通信。通过 9 个场景标签页，全面覆盖 MQTT 协议的核心功能与高级用法，包括发布/订阅、QoS 级别、主题通配符、遗嘱消息、保留消息、认证与 TLS、请求-响应模式、批量发布和消息监控。

## 知识点

- `QMqttClient` 客户端连接与管理
- `QMqttSubscription` 订阅管理
- `QMqttMessage` 消息收发
- `QMqttTopicFilter` 主题过滤与通配符
- QoS 0 / 1 / 2 三种服务质量等级
- Last Will and Testament（LWT）遗嘱消息
- Retained Message 保留消息
- Username / Password 认证
- TLS/SSL 安全连接
- 请求-响应（RPC）模式
- 批量发布与性能统计
- 自动重连机制（指数退避）

## 默认 Broker

本 demo 默认使用公共测试 Broker：
- **MQTT**: `broker.emqx.io:1883`
- **WebSocket**: `broker.emqx.io:8083/mqtt`

> 也可替换为本地 Mosquitto 等 Broker。

## 环境要求

```bash
# 系统依赖
sudo apt install qtbase5-dev qtbase5-private-dev

# Qt MQTT 模块
# 本 demo 内置了从源码编译的 Qt MQTT 库（third_party/qt-mqtt/），
# 无需额外安装。如需重新编译该库，见下方"重新编译 Qt MQTT"章节。
```

## 重新编译 Qt MQTT 库（一般不需要）

如需重新编译 third_party 中的 Qt MQTT 库：

```bash
# 1. 克隆 Qt MQTT 5.15 源码
cd /tmp && git clone --depth 1 --branch v5.15.2 https://code.qt.io/qt/qtmqtt.git

# 2. 编译（需要 qtbase5-dev + qtbase5-private-dev）
# 使用 third_party/build_mqtt.sh 或手动执行
bash third_party/build_mqtt.sh
```

## 场景列表

| Tab | 场景 | 说明 |
|-----|------|------|
| 1 | 基础发布/订阅 | connect → subscribe → publish → receive 完整流程 |
| 2 | QoS 级别 | QoS 0/1/2 对比演示，传输状态追踪 |
| 3 | 主题与通配符 | 单级 `+`、多级 `#` 通配符匹配 |
| 4 | Last Will 遗嘱 | LWT 配置与意外断开模拟 |
| 5 | 保留消息 | Retained 消息发布、接收与清除 |
| 6 | 认证与 TLS | 用户名密码认证、TLS/SSL 安全连接 |
| 7 | 请求-响应 | 基于 MQTT 的 RPC 模式实现 |
| 8 | 批量发布 | 文件加载、间隔发布、性能统计 |
| 9 | 消息监控仪表盘 | 实时流量、连接状态、消息统计 |

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h/cpp | 主窗口（左侧连接面板 + 右侧 TabWidget） |
| src/mqtt_client_wrapper.h/cpp | QMqttClient 封装（自动重连、日志、统一接口） |
| src/tab_publish_subscribe.h/cpp | Tab 1: 基础发布/订阅 |
| src/tab_qos_level.h/cpp | Tab 2: QoS 级别演示 |
| src/tab_topic_wildcard.h/cpp | Tab 3: 主题与通配符 |
| src/tab_lwt.h/cpp | Tab 4: Last Will 遗嘱消息 |
| src/tab_retained.h/cpp | Tab 5: 保留消息 |
| src/tab_auth_tls.h/cpp | Tab 6: 认证与 TLS/SSL |
| src/tab_request_response.h/cpp | Tab 7: 请求-响应模式 |
| src/tab_batch_publish.h/cpp | Tab 8: 批量发布 |
| src/tab_message_monitor.h/cpp | Tab 9: 消息监控仪表盘 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```
