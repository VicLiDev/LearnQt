# 05.dialogs -- 对话框

## 概述
本示例演示Qt标准对话框和自定义对话框的使用。包括文件选择、消息提示、颜色选择、输入对话框，以及自定义的AboutDialog关于对话框。

## 知识点
- QFileDialog 文件选择对话框（打开/保存）
- QMessageBox 消息对话框（信息/警告/确认/错误）
- QColorDialog 颜色选择对话框
- QInputDialog 输入对话框
- QDialog 自定义对话框的创建与模态/非模态显示
- QDialogButtonBox 按钮组

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
| src/mainwindow.cpp | 各标准对话框的调用示例 |
| src/aboutdialog.h | 自定义AboutDialog声明 |
| src/aboutdialog.cpp | 自定义AboutDialog实现 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
