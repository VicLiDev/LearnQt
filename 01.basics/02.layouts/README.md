# 01.layouts -- 布局管理

## 概述
本示例演示Qt四种基本布局管理器的用法：垂直布局（QVBoxLayout）、水平布局（QHBoxLayout）、网格布局（QGridLayout）和表单布局（QFormLayout）。通过QTabWidget将不同布局组织在选项卡中展示，便于对比学习。

## 知识点
- QVBoxLayout 垂直布局
- QHBoxLayout 水平布局
- QGridLayout 网格布局（行列跨越）
- QFormLayout 表单布局（标签+输入框组合）
- QTabWidget 选项卡容器
- 布局嵌套与间距控制

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
| src/mainwindow.cpp | 四种布局的创建与配置 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
