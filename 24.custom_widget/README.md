# 24.custom_widget -- 自定义控件

## 概述

演示 Qt5 中自定义控件的多种实现方式：继承 QWidget 重写 paintEvent 绘制圆形按钮、进度仪表盘；继承 QStyledItemDelegate 实现 QTableView 中的自定义渲染与编辑；以及将多个标准 Widget 组合封装为可复用的复合控件。

## 知识点

- **自定义绘制控件**：继承 QWidget，重写 `paintEvent()`、`sizeHint()`、`minimumSizeHint()`
- **鼠标事件处理**：重写 `mousePressEvent()`、`mouseReleaseEvent()`、`enterEvent()`、`leaveEvent()`
- **自定义信号**：使用 `signals:` 定义信号，通过 `emit` 发送状态变化通知
- **QPainter 绘图**：圆形、渐变、阴影、路径、文字绘制
- **QTimer 动画**：使用定时器实现值渐变动画，缓出曲线
- **QStyledItemDelegate**：重写 `paint()`、`sizeHint()`、`createEditor()`、`setModelData()`
- **QPainterPath**：自定义五角星等复杂形状的路径绘制
- **组合控件设计**：封装多个标准 Widget，暴露统一的信号槽接口
- **Q_PROPERTY**：声明自定义属性以支持动画和元对象系统

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h/cpp | 主窗口，包含四个 Tab 页的布局和逻辑 |
| src/circle_button.h/cpp | 自定义圆形按钮控件（paintEvent、鼠标事件、自定义信号） |
| src/gauge_widget.h/cpp | 进度仪表盘控件（半圆弧、刻度、指针、QTimer 动画） |
| src/star_delegate.h/cpp | 自定义委托（进度条、星级评分、颜色块渲染与编辑） |
| src/file_path_selector.h/cpp | 组合控件：文件路径选择器 + 搜索框 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
./prjBuild.sh rebuild
```
