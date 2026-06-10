# 23.ipc -- 进程间通信

## 概述
本示例演示 Qt5 进程间通信(IPC)的各种方式，包括共享内存(QSharedMemory)、本地套接字(QLocalServer/QLocalSocket)、进程管理(QProcess)，以及综合示例演示进程间数据传递。

## 知识点
- QSharedMemory 创建/附加/分离共享内存
- 共享内存数据读写与大小限制
- QLocalServer 监听与 QLocalSocket 连接
- 双向通信与多客户端管理
- 断开连接检测
- QProcess 启动外部程序
- 读取标准输出/标准错误
- 写入标准输入
- 等待完成与获取退出码
- 环境变量设置
- 共享内存 + 本地套接字综合应用

## 构建与运行
```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```

## 关键文件
| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h | MainWindow类声明 |
| src/mainwindow.cpp | 进程间通信UI及逻辑实现 |
| CMakeLists.txt | CMake构建配置 |
