# 10.file_io -- 文件读写与配置

## 概述
本示例演示Qt文件I/O操作，包括QFile与QTextStream读写文本文件，QFileDialog文件选择，以及使用QSettings保存窗口几何设置和最近打开文件列表，实现配置持久化。

## 知识点
- QFile 文件打开、读取、写入
- QTextStream 文本流读写（逐行/全部）
- QFileDialog 文件选择（打开/保存/过滤）
- QSettings 配置存储（INI/注册表格式）
- 保存/恢复窗口大小与位置
- 最近文件列表（MRU）管理

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
| src/mainwindow.cpp | 文件读写、QSettings配置保存 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
