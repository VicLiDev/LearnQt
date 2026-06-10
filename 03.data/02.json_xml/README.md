# 18.json_xml -- JSON/XML 数据处理

## 概述

学习使用 Qt5 Core 模块中的 JSON 和 XML 处理类。演示了 QJsonObject/QJsonArray/QJsonDocument 的创建、读写、序列化与反序列化，以及 QDomDocument 的 XML 树操作、XPath 查询等功能。最后对 JSON 和 XML 两种数据格式进行对比分析。

## 知识点

- `QJsonObject` / `QJsonArray` / `QJsonValue` JSON 数据结构操作
- `QJsonDocument` 序列化（toJson）与反序列化（fromJson）
- JSON 文件读写与格式化显示
- 嵌套 JSON 解析与 JSON Path 手动查询
- JSON 校验与 QVariant 转换
- `QDomDocument` XML 文档树构建
- `QDomElement` / `QDomNode` 节点操作
- XML 文件读写与格式化显示
- XPath 风格查询（elementsByTagName）
- XML 属性读取与修改
- JSON 与 XML 格式对比与性能分析

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h | 主窗口头文件，声明所有标签页 Widget |
| src/mainwindow.cpp | 主窗口实现，五个标签页的完整功能 |

## 构建与运行

```bash
./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```
