# 09.model_view -- 模型与视图

## 概述
本示例演示Qt模型/视图架构（Model/View）的使用。包括QTableView配合QStandardItemModel表格展示、QListView配合QStringListModel列表展示、QTreeView配合树形模型展示三种典型场景。

## 知识点
- Model/View 架构基本概念（Model / View / Delegate）
- QStandardItemModel 通用项模型（表格数据）
- QStringListModel 字符串列表模型
- QTreeView 树形视图与层级模型
- QTableView / QListView / QTreeView 视图控件
- 模型数据的增删改查操作

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
| src/mainwindow.cpp | 三种模型视图的创建与数据填充 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
