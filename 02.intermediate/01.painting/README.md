# 07.painting -- QPainter绘图

## 概述
本示例展示QPainter 2D绑图能力，包括基本图形（线、矩形、圆、弧）绘制、文字渲染、渐变填充，以及使用QTimer驱动动画实现图形旋转效果。

## 知识点
- QPainter 绑图设备与绑图操作
- 绘制线段、矩形、圆形、弧形
- 绘制文字（setFont / drawText）
- QLinearGradient / QRadialGradient 渐变填充
- QPen 画笔设置（颜色、宽度、线型）
- QBrush 画刷设置
- QTimer 定时器驱动动画刷新
- update() 触发重绘

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
| src/mainwindow.cpp | 主窗口布局，PaintWidget集成 |
| src/paintwidget.h | PaintWidget自定义绑图控件声明 |
| src/paintwidget.cpp | 绘图操作与旋转动画实现 |
| CMakeLists.txt | CMake构建配置 |
