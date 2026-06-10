# 02.basic_widgets -- 常用控件

## 概述
本示例集中展示Qt常用UI控件的创建与使用，包括标签、输入框、下拉框、单选按钮、复选框、滑块、进度条和日期编辑等。是Qt控件学习的速查参考。

## 知识点
- QLabel 显示文本与图片
- QLineEdit 单行文本输入
- QTextEdit 多行文本编辑
- QSpinBox / QComboBox 数字选择与下拉列表
- QRadioButton / QCheckBox 单选与复选
- QGroupBox 分组容器
- QSlider / QProgressBar 滑块与进度条
- QDateEdit 日期选择控件

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
| src/mainwindow.cpp | 各控件的创建、布局与信号连接 |
| src/mainwindow.ui | UI设计文件 |
| CMakeLists.txt | CMake构建配置 |
