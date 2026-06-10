# 02.custom_plot_events -- 绘图 + 事件系统 + 不规则窗口

## 概述

综合 demo，同时使用 QWT 和 QCustomPlot 两个绘图库，深入学习 Qt 事件系统，以及实现不规则形状窗口。涵盖绘图、事件处理、UI 资源等核心 Qt 概念。

## 知识点

### 绘图
- QWT 图表：曲线、网格、缩放、平移、图例、标记点
- QCustomPlot 绑定的第三方库直接编译
- Qt 资源系统 (.qrc)

### 事件系统
- `event()` 事件分发
- `eventFilter()` 事件过滤器安装与拦截
- `mousePressEvent()` / `mouseMoveEvent()` 鼠标事件
- `keyPressEvent()` 键盘事件
- `paintEvent()` 自定义绘制（不规则窗口）

### 不规则窗口
- `QPainterPath` + `setMask()` 实现非矩形窗口
- 鼠标拖拽移动无标题栏窗口
- `FramelessWindowHint` 无边框窗口标志

## 关键文件

| 文件 | 说明 |
|------|------|
| src/mainwindow.h/cpp | 主窗口：QWT 绘图 + 事件处理 + QCustomPlot 绘图 |
| src/dialog.h/cpp/ui | 普通对话框 |
| src/irregulardialog.h/cpp/ui | 不规则形状对话框（自定义绘制） |
| src/qcustomplot.h/cpp | QCustomPlot 第三方库源码 |
| resources/myresource.qrc | Qt 资源文件 |

## 构建与运行

```bash
# 需要 QWT 库
QWT_CMAKE_ARGS="-DQWT_ROOT=/your/qwt/path" ./prjBuild.sh build

./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```
