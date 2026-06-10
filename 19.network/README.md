# 19.network -- 网络编程

## 概述

学习使用 Qt5 Network 模块进行网络编程。演示了 TCP 客户端/服务端通信、UDP 数据报收发、HTTP 请求处理，以及网络接口信息查询等功能。

## 知识点

- `QTcpSocket` TCP 客户端连接、发送/接收消息
- `QTcpServer` TCP 服务端监听端口、接受连接、广播消息
- `QUdpSocket` UDP 数据报发送/接收，广播/单播/组播
- `QNetworkAccessManager` HTTP GET/POST 请求
- `QNetworkReply` 响应处理与数据读取
- `QNetworkInterface` 获取本机网络接口与 IP 地址
- `QNetworkConfigurationManager` 在线状态检测
- 网络连接状态管理与断线重连

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h | 主窗口头文件，声明所有标签页 Widget |
| src/mainwindow.cpp | 主窗口实现，五个标签页的完整功能 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```

## 使用说明

- TCP 服务端默认监听端口 12345
- 可以在 TCP 客户端标签页连接 localhost:12345 进行自测
- TCP 客户端也可以连接同一标签页中的服务端
- HTTP 功能需要目标服务器可访问
