# 20.graphics_view -- Graphics View 图形框架

## 概述

学习使用 Qt5 Graphics View 框架进行图形编程。演示了 QGraphicsScene + QGraphicsView 的基本图形绘制、交互与选中、碰撞检测、坐标变换，以及自定义 QGraphicsItem 的实现。

## 知识点

- `QGraphicsScene` 场景管理与图形项添加
- `QGraphicsView` 视图显示与交互控制
- `QGraphicsRectItem` / `QGraphicsEllipseItem` / `QGraphicsTextItem` / `QGraphicsLineItem` 基本图形项
- 图形项拖拽移动、缩放、旋转
- 鼠标事件处理（hoverEvent / mousePressEvent / mouseMoveEvent / mouseReleaseEvent）
- 项目选中高亮与右键菜单
- 框选多个项目（Rubber Band Selection）
- `QGraphicsItem::collidesWithItem` 碰撞检测
- `QGraphicsItem::setRotation` / `setScale` / `setTransform` 坐标变换
- 场景坐标、视图坐标、项目坐标的相互转换
- 继承 `QGraphicsObject` 实现自定义绘图（paint / boundingRect）
- 自定义可拖拽流程图节点与连接端口

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h | 主窗口头文件，声明所有标签页 Widget 及自定义 Item |
| src/mainwindow.cpp | 主窗口实现，五个标签页的完整功能与自定义图形项 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```
