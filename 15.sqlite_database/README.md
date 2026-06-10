# 03.sqlite_database -- SQLite 数据库操作

## 概述

学习使用 Qt SQL 模块操作 SQLite 数据库。演示了数据库连接封装、表创建、CRUD（增删改查）操作，以及自定义数据库 Widget 的使用。

## 知识点

- `QSqlDatabase` 数据库连接管理
- `QSqlQuery` 执行 SQL 语句
- `QSqlError` 错误处理
- `QSqlRecord` / `QSqlField` 字段操作
- 数据库操作类的封装设计（DataBaseEncap）
- 自定义 Widget 展示数据库内容（DataBaseWidget）
- 用户业务逻辑层（DataBaseUserWork）

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h/cpp/ui | 主窗口 |
| src/databaseencap.h/cpp | 数据库连接封装类（连接、建表、增删改查） |
| src/databaseuserwork.h/cpp | 用户业务逻辑层 |
| src/databasewidget.h/cpp/ui | 数据库展示 Widget |
| data/ | 运行时数据库文件存放目录 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```
