# 03.signals_slots -- 信号与槽

## 概述
本示例深入讲解Qt核心机制——信号与槽。自定义Counter类演示信号的声明与发射，展示了connect/disconnect、信号连接信号、lambda与传统写法对比等多种用法。

## 知识点
- 自定义信号声明（`signals:`）与发射（`emit`）
- 自定义槽函数声明与实现
- `connect` 连接信号槽（lambda与传统写法）
- `disconnect` 断开连接
- 信号连接信号（信号链式传递）
- QObject::connect的第五个参数（连接类型）

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
| src/counter.h | Counter自定义类声明，含自定义信号 |
| src/counter.cpp | Counter类实现，值变化时发射信号 |
| src/mainwindow.h | MainWindow类声明 |
| src/mainwindow.cpp | 多种信号槽连接方式演示 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
