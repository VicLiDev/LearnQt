# LearnQt - Qt5 学习项目

从基础到进阶的 Qt5 学习 demo 集合，涵盖 C++ (Qt5) 和 Python (PyQt5) 两个方向。

## 环境要求

- **编译器**: GCC / Clang (C++11)
- **Qt5**: `apt install qtbase5-dev qtbase5-dev-tools qtbase5-private-dev libqt5xml5-dev libqt5printsupport5-dev libqt5networkauth5-dev`
- **CMake**: `apt install cmake` (3.5+)
- **PyQt5** (仅 16 号 demo): `pip3 install PyQt5 psutil`
- **QWT** (13, 14 号 demo): 需要安装 QWT 库，通过 `-DQWT_ROOT=/path` 指定路径
- **Qt MQTT** (17 号 demo): `apt install libqt5mqtt5-dev`

## Demo 列表

### Qt C++ 基础教程 (01-12)

| 编号 | 目录 | 说明 |
|------|------|------|
| 01 | hello_qt | 最简 Qt 程序、QPushButton、信号槽（connect + lambda） |
| 02 | layouts | QVBoxLayout / HBoxLayout / GridLayout / FormLayout |
| 03 | basic_widgets | QLabel、QLineEdit、QTextEdit、QSpinBox、QComboBox、QRadioButton、QCheckBox 等常用控件 |
| 04 | signals_slots | 自定义信号槽、connect/disconnect、信号连接信号、lambda 与传统写法 |
| 05 | mainwindow | QMainWindow 框架：MenuBar、ToolBar、StatusBar、QAction、快捷键 |
| 06 | dialogs | 标准对话框（QFileDialog、QMessageBox、QColorDialog、QInputDialog）、自定义对话框 |
| 07 | events | 事件系统：event()、eventFilter()、鼠标/键盘/滚轮事件、无边框窗口拖拽 |
| 08 | painting | QPainter 绘图：线、矩形、圆、弧、文字、渐变，QTimer 动画 |
| 09 | timer_thread | QTimer 定时器、QThread + moveToThread 多线程、跨线程信号槽 |
| 10 | model_view | QTableView / QListView / QTreeView 与 Model 架构 |
| 11 | file_io | QFile / QTextStream 读写、QFileDialog、QSettings 窗口设置持久化 |
| 12 | style_qss | QStyleFactory 风格切换、QSS 样式表、深色/浅色主题 |

### 第三方库与进阶 (13-16)

| 编号 | 目录 | 说明 | 依赖 |
|------|------|------|------|
| 13 | qwt_plotting | QWT 绘图库：曲线、网格、缩放、图例交互 | QWT |
| 14 | custom_plot_advanced | QCustomPlot + QWT + 事件系统 + 不规则窗口 | QWT |
| 15 | sqlite_database | Qt SQL 模块、SQLite 封装、CRUD、多线程 | Qt5::Sql |
| 16 | pyqt_system_monitor | PyQt5 系统监控仪表盘、无边框窗口、事件过滤器 | PyQt5 + psutil |
| 17 | mqtt | Qt MQTT 综合学习：发布订阅、QoS、通配符、LWT、保留消息、TLS、RPC、批量发布、监控仪表盘 | Qt5::Mqtt |

### Qt 进阶教程 (18-24)

| 编号 | 目录 | 说明 |
|------|------|------|
| 18 | json_xml | JSON/XML 数据解析：QJsonDocument、QJsonParseError、QDomDocument、QXmlStreamReader |
| 19 | network | 网络编程：QTcpSocket/QTcpServer、QUdpSocket、QNetworkAccessManager HTTP 请求 |
| 20 | graphics_view | Graphics View 图形视图框架：QGraphicsScene、QGraphicsItem、坐标系统、碰撞检测 |
| 21 | animation | Qt 动画框架：QPropertyAnimation、QParallelAnimationGroup、QSequentialAnimationGroup、 easing |
| 22 | concurrent | Qt 并发编程：QtConcurrent::map、QFuture、QFutureWatcher、线程池 |
| 23 | ipc | 进程间通信：QProcess、QLocalSocket/QLocalServer、QSharedMemory |
| 24 | custom_widget | 自定义控件：绘制控件（圆形按钮/仪表盘）、QStyledItemDelegate（进度条/星评/颜色块）、组合控件 |

## 快速开始

```bash
# 构建所有 demo
make all

# 构建单个 demo
make 01.hello_qt
make 15.sqlite_database

# 构建并运行
make run-01.hello_qt

# 清理
make clean

# 查看所有 demo
make list
```

也可以直接进入 demo 目录：
```bash
cd 01.hello_qt
./prjBuild.sh build   # 编译
./prjBuild.sh run     # 编译并运行
./prjBuild.sh clean   # 清理
./prjBuild.sh rebuild # 重新编译
```

## 学习路线

1. **01.hello_qt** → 从最简单的程序开始，理解信号槽
2. **02.layouts** → 学习布局管理，构建界面
3. **03.basic_widgets** → 熟悉常用控件
4. **04.signals_slots** → 深入理解 Qt 核心机制：信号与槽
5. **05.mainwindow** → 构建完整的主窗口应用程序
6. **06.dialogs** → 学习标准对话框和自定义对话框
7. **07.events** → 掌握 Qt 事件系统
8. **08.painting** → 学习 2D 图形绘制
9. **09.timer_thread** → 定时器与多线程
10. **10.model_view** → Model/View 架构
11. **11.file_io** → 文件操作与设置持久化
12. **12.style_qss** → 界面美化与样式系统
13. **13-14** → 第三方绘图库（需要安装 QWT）
14. **15** → 数据库编程
15. **16** → Python + PyQt5 实战
16. **17** → MQTT 物联网通信
17. **18** → JSON/XML 数据解析
18. **19** → 网络编程（TCP/UDP/HTTP）
19. **20** → Graphics View 图形视图框架
20. **21** → Qt 动画框架
21. **22** → Qt 并发编程
22. **23** → 进程间通信
23. **24** → 自定义控件（绘制/委托/组合控件）
