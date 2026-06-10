# 04.mainwindow -- 主窗口框架

## 概述
本示例展示QMainWindow主窗口框架的搭建，包括菜单栏、工具栏、状态栏的创建，QAction的动作绑定，快捷键设置，以及中央放置QTextEdit文本编辑区域。

## 知识点
- QMainWindow 三栏结构（MenuBar / ToolBar / StatusBar）
- QAction 动作创建与菜单/工具栏绑定
- 快捷键（QKeySequence）设置
- QTextEdit 作为中央控件
- 菜单项的分组与分隔符
- 状态栏实时信息显示

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
| src/mainwindow.cpp | 菜单栏/工具栏/状态栏/Action创建 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
