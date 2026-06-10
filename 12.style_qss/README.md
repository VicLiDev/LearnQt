# 11.style_qss -- 样式与QSS

## 概述
本示例展示Qt应用程序的外观定制方法。包括使用QStyleFactory切换系统内置风格，通过QSS样式表（类似CSS语法）自定义控件外观，以及深色/浅色主题的切换实现。

## 知识点
- QStyleFactory 切换系统内置风格（Fusion / Windows等）
- QSS样式表语法与加载方式
- QWidget::setStyleSheet 设置样式
- QSS选择器（类型/ID/属性选择器）
- 深色主题与浅色主题的设计与切换
- Qt资源文件（.qrc）管理QSS文件

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
| src/mainwindow.cpp | 风格切换与QSS加载逻辑 |
| src/mainwindow.ui | UI设计文件 |
| resources/style.qss | QSS样式表文件 |
| resources/style.qrc | Qt资源文件 |
| CMakeLists.txt | CMake构建配置 |
