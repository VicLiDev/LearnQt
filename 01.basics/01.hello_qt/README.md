# 00.hello_qt -- 最简Qt程序

## 概述
本示例演示Qt编程的入门基础，创建一个带有按钮和标签的窗口，通过信号槽机制实现点击按钮更新计数器。展示了Qt程序的基本结构和`connect`函数配合lambda表达式的用法。

## 知识点
- QPushButton 与 QLabel 的创建与基本使用
- `connect(sender, signal, receiver, slot)` 信号槽连接
- lambda 表达式作为槽函数
- QWidget 基本窗口设置（标题、大小、布局）

## 构建与运行
```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```

## 关键文件
| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口，创建QApplication与主窗口 |
| src/mainwindow.h | MainWindow类声明 |
| src/mainwindow.cpp | MainWindow实现，按钮/标签/计数器逻辑 |
| CMakeLists.txt | CMake构建配置 |
