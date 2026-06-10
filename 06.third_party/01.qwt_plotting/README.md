# 01.qwt_plotting -- QWT 绘图库

## 概述

学习 QWT (Qt Widgets for Technical Applications) 第三方绘图库的基本使用。演示了多种绘图元素：曲线、网格、缩放/平移、图例交互、坐标轴定制等。

## 知识点

- QWT 库的集成与 CMake 配置
- QwtPlot 基本绘图框架
- QwtPlotCurve 曲线绘制（折线图、平滑曲线）
- QwtPlotGrid 网格线
- QwtPlotMagnifier / QwtPlotPanner / QwtPlotZoomer 缩放与平移
- QwtLegend 图例（可点击切换显示）
- QwtPlotMarker 标记点
- 自定义坐标轴缩放 (myqwtscaledraw)
- QTimer 定时刷新数据

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.cpp | 程序入口 |
| src/mainwindow.h/cpp | 主窗口，包含 4 种 QWT 图表类型 |
| src/mainwindow.ui | UI 设计文件 |
| src/myqwtscaledraw.cpp | 自定义坐标轴缩放绘制 |

## 构建与运行

```bash
# 需要 QWT 库，默认搜索 /usr/local/qwt-6.1.3/
# 可通过 QWT_ROOT 指定路径:
QWT_CMAKE_ARGS="-DQWT_ROOT=/your/qwt/path" ./prjBuild.sh build

./prjBuild.sh build
./prjBuild.sh run
./prjBuild.sh clean
```

## QWT 图例点击参考代码

参见 `README_ref.txt` -- 图例可点击控制曲线显示/隐藏。

## 参考资料

- QWT 官方文档: https://qwt.sourceforge.io/
- QWT 示例: qwt-6.1.3/examples/
