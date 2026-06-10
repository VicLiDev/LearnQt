# 08.timer_thread -- 定时器与多线程

## 概述
本示例演示QTimer实现倒计时进度条功能，以及QThread+moveToThread的跨线程工作模式。展示Worker对象在子线程中执行耗时任务，通过信号槽与主线程安全通信。

## 知识点
- QTimer 定时器创建与超时处理
- QThread 线程类使用
- moveToThread 将Worker对象移至子线程
- 跨线程信号槽通信（自动队列连接）
- QMutex / 互斥锁保护共享数据（可选）
- 线程生命周期管理（start/quit/wait）

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
| src/mainwindow.cpp | QTimer倒计时+进度条UI |
| src/worker.h | Worker工作类声明，定义任务信号槽 |
| src/worker.cpp | Worker耗时任务实现 |
| CMakeLists.txt | CMake构建配置 |
