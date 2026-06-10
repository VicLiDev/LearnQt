# 06.events -- 事件系统

## 概述
本示例讲解Qt事件处理机制，包括事件分发、事件过滤器、鼠标/键盘/滚轮事件的捕获与处理。自定义PaintArea控件演示鼠标拖拽绘制轨迹和按键响应。

## 知识点
- QEvent 事件基类与事件分发流程
- eventFilter 事件过滤器安装与使用
- mouseMoveEvent / mousePressEvent / mouseReleaseEvent 鼠标事件
- keyPressEvent 键盘按键事件
- wheelEvent 滚轮事件
- setMouseTracking 鼠标追踪设置
- 自定义QWidget子类重写事件处理函数

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
| src/mainwindow.cpp | 事件过滤器与各类事件演示 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
