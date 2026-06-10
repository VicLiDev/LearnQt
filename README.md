# LearnQt - Qt5 学习项目

从基础到进阶的 Qt5 学习 demo 集合，涵盖 C++ (Qt5) 和 Python (PyQt5) 两个方向。

## 环境要求

- **编译器**: GCC / Clang (C++11)
- **Qt5**: `apt install qtbase5-dev qtbase5-dev-tools libqt5xml5-dev libqt5printsupport5-dev`
- **CMake**: `apt install cmake` (3.5+)
- **PyQt5** (仅 06.third_party/03): `pip3 install PyQt5 psutil`
- **QWT** (06.third_party/01, 02): 需要安装 QWT 库，通过 `-DQWT_ROOT=/path` 指定路径

## Demo 列表

### 01.basics — 基础篇

| 编号 | 目录 | 说明 |
|------|------|------|
| 01 | hello_qt | 最简 Qt 程序、QPushButton、信号槽（connect + lambda） |
| 02 | layouts | QVBoxLayout / HBoxLayout / GridLayout / FormLayout |
| 03 | basic_widgets | QLabel、QLineEdit、QTextEdit、QSpinBox、QComboBox、QRadioButton、QCheckBox 等常用控件 |
| 04 | signals_slots | 自定义信号槽、connect/disconnect、信号连接信号、lambda 与传统写法 |
| 05 | mainwindow | QMainWindow 框架：MenuBar、ToolBar、StatusBar、QAction、快捷键 |
| 06 | dialogs | 标准对话框（QFileDialog、QMessageBox、QColorDialog、QInputDialog）、自定义对话框 |
| 07 | events | 事件系统：event()、eventFilter()、鼠标/键盘/滚轮事件、无边框窗口拖拽 |

### 02.intermediate — 进阶篇

| 编号 | 目录 | 说明 |
|------|------|------|
| 01 | painting | QPainter 绘图：线、矩形、圆、弧、文字、渐变，QTimer 动画 |
| 02 | timer_thread | QTimer 定时器、QThread + moveToThread 多线程、跨线程信号槽 |
| 03 | model_view | QTableView / QListView / QTreeView 与 Model 架构 |
| 04 | file_io | QFile / QTextStream 读写、QFileDialog、QSettings 窗口设置持久化 |
| 05 | style_qss | QStyleFactory 风格切换、QSS 样式表、深色/浅色主题 |

### 03.data — 数据处理篇

| 编号 | 目录 | 说明 | 依赖 |
|------|------|------|------|
| 01 | sqlite_database | Qt SQL 模块、SQLite 封装、CRUD、多线程 | Qt5::Sql |
| 02 | json_xml | QJsonDocument、QJsonObject/QJsonArray、QDomDocument、XML 解析与生成、JSON vs XML 对比 | Qt5::Xml |

### 04.graphics_animation — 图形与动画篇

| 编号 | 目录 | 说明 |
|------|------|------|
| 01 | graphics_view | QGraphicsScene / QGraphicsView / QGraphicsItem、碰撞检测、坐标变换、自定义 Item |
| 02 | animation | QPropertyAnimation、动画组（串行/并行）、缓动曲线、QStateMachine 状态机 |
| 03 | custom_widget | 自定义绘制控件（圆形按钮/仪表盘）、QStyledItemDelegate（进度条/星评）、组合控件 |

### 05.network_concurrent — 网络与并发篇

| 编号 | 目录 | 说明 | 依赖 |
|------|------|------|------|
| 01 | mqtt | Qt MQTT 综合学习：发布订阅、QoS、通配符、LWT、保留消息、TLS、RPC、批量发布、监控 | Qt5::Mqtt |
| 02 | network | QTcpSocket/QTcpServer、QUdpSocket、QNetworkAccessManager HTTP 请求、网络信息 | Qt5::Network |
| 03 | concurrent | QtConcurrent::run/map/filter、QFuture、QFutureWatcher、QThreadPool | Qt5::Concurrent |
| 04 | ipc | QSharedMemory、QLocalSocket/QLocalServer、QProcess 进程管理 | Qt5::Network |

### 06.third_party — 第三方库与特殊

| 编号 | 目录 | 说明 | 依赖 |
|------|------|------|------|
| 01 | qwt_plotting | QWT 绘图库：曲线、网格、缩放、图例交互 | QWT |
| 02 | custom_plot_advanced | QCustomPlot + QWT + 事件系统 + 不规则窗口 | QWT |
| 03 | pyqt_system_monitor | PyQt5 系统监控仪表盘、无边框窗口、事件过滤器 | PyQt5 + psutil |

## 快速开始

```bash
# 批量操作
make all         # 构建所有 demo
make clean       # 清理所有 demo
make list        # 列出所有 demo（按分类显示）

# 单个 demo（进入目录操作）
cd 01.basics/01.hello_qt
./prjBuild.sh build   # 编译
./prjBuild.sh run     # 编译并运行
./prjBuild.sh clean   # 清理
./prjBuild.sh rebuild # 重新编译
```

## 学习路线

### 基础篇
1. **01.hello_qt** → 从最简单的程序开始，理解信号槽
2. **02.layouts** → 学习布局管理，构建界面
3. **03.basic_widgets** → 熟悉常用控件
4. **04.signals_slots** → 深入理解 Qt 核心机制：信号与槽
5. **05.mainwindow** → 构建完整的主窗口应用程序
6. **06.dialogs** → 学习标准对话框和自定义对话框
7. **07.events** → 掌握 Qt 事件系统

### 进阶篇
8. **01.painting** → 学习 2D 图形绘制
9. **02.timer_thread** → 定时器与多线程
10. **03.model_view** → Model/View 架构
11. **04.file_io** → 文件操作与设置持久化
12. **05.style_qss** → 界面美化与样式系统

### 数据处理篇
13. **01.sqlite_database** → 数据库编程
14. **02.json_xml** → JSON/XML 数据解析

### 图形与动画篇
15. **01.graphics_view** → Graphics View 图形视图框架
16. **02.animation** → Qt 动画框架
17. **03.custom_widget** → 自定义控件（绘制/委托/组合控件）

### 网络与并发篇
18. **02.network** → 网络编程（TCP/UDP/HTTP）
19. **03.concurrent** → Qt 并发编程
20. **01.mqtt** → MQTT 物联网通信
21. **04.ipc** → 进程间通信

### 第三方库
22. **01.qwt_plotting / 02.custom_plot_advanced** → 第三方绘图库（需要安装 QWT）
23. **03.pyqt_system_monitor** → Python + PyQt5 实战
