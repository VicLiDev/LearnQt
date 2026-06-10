# 04.pyqt_system_monitor -- PyQt5 系统监控仪表盘

## 概述

使用 Python + PyQt5 实现的系统状态监控工具，显示 CPU、内存、网络流量和温度信息。同时包含多个 PyQt5 小 demo，覆盖信号槽、事件处理、无边框窗口等核心概念。

## 知识点

### 主程序 (src/)
- PyQt5 Widget 基础（QWidget、QLabel、QGridLayout）
- QTimer 定时器刷新数据
- psutil 库获取系统信息
- 无边框窗口 (`FramelessWindowHint`)
- 窗口置顶 (`WindowStaysOnTopHint`)
- 事件过滤器 (eventFilter) 实现鼠标拖拽
- 事件系统 (event, mousePressEvent, mouseMoveEvent, keyPressEvent)
- `mapToGlobal()` / `mapFromGlobal()` 坐标转换
- 多线程避免 UI 卡顿

### Demo 集 (demos/)
- **UnFrameWindow**: 无边框窗口样式（QSS）
- **signalAndSlots**: 信号与槽机制
- **eventAndSignal**: 自定义信号、事件对象、事件源
- **usingDesigner**: 使用 Qt Designer 设计 UI 并转换为 Python 代码
- **net_traffic.py**: 网络流量监控独立脚本

## 环境要求

```bash
pip3 install PyQt5 psutil
# 或使用 requirements.txt:
pip3 install -r requirements.txt
```

## 关键文件

| 文件 | 说明 |
|------|------|
| src/main.py | 主程序入口，组合三个监控 Widget |
| src/pyqtCpuMemWidget.py | CPU/内存监控 Widget |
| src/pyqtNetWidget.py | 网络流量监控 Widget |
| src/pyqtCpuTempWidget.py | CPU 温度监控 Widget |
| src/cpumeminfo.py | CPU/内存信息获取 |
| src/netinfo.py | 网络信息获取 |
| src/temperatures.py | 温度信息获取 |

## 构建与运行

```bash
./prjBuild.sh build   # 检查依赖
./prjBuild.sh run     # 运行
./prjBuild.sh clean   # 清理 __pycache__

# 运行子 demo
python3 demos/signalAndSlots/sigslot.py
python3 demos/eventAndSignal/eventsource.py
python3 demos/net_traffic.py
```

## 参考资料

- PyQt5 信号与槽: https://www.tutorialspoint.com/pyqt/pyqt_signals_and_slots.htm
- PyQt5 教程: https://likegeeks.com/pyqt5-tutorial/

### 窗口尺寸设置方法

```python
resize(w, h)              # 设置宽高
setGeometry(x, y, w, h)   # 设置位置和宽高
setFixedSize(w, h)         # 固定尺寸（不可拖拽）
setMinimumSize(w, h)       # 最小尺寸
setMaximumSize(w, h)       # 最大尺寸
```
