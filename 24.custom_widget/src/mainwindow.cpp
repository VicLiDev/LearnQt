#include "mainwindow.h"
#include "circle_button.h"
#include "gauge_widget.h"
#include "star_delegate.h"
#include "file_path_selector.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QFont>

// ============================================================
// 构造函数 —— 初始化主窗口和各 Tab 页
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("24.自定义控件 - Qt5 学习 Demo"));
    resize(900, 700);

    // 创建中心 Tab 控件
    mTabWidget = new QTabWidget;
    setCentralWidget(mTabWidget);

    // 创建各 Tab 页
    QWidget *tab1 = new QWidget;
    QWidget *tab2 = new QWidget;
    QWidget *tab3 = new QWidget;
    QWidget *tab4 = new QWidget;

    mTabWidget->addTab(tab1, tr("自定义绘制控件"));
    mTabWidget->addTab(tab2, tr("进度仪表盘"));
    mTabWidget->addTab(tab3, tr("自定义 Delegate"));
    mTabWidget->addTab(tab4, tr("组合控件"));

    // 初始化各 Tab 页内容
    setupTab1CircleButton(tab1);
    setupTab2GaugeWidget(tab2);
    setupTab3Delegate(tab3);
    setupTab4CompositeWidget(tab4);
}

MainWindow::~MainWindow()
{
}

// ============================================================
// Tab 1: 自定义绘制控件 —— 圆形按钮演示
// ============================================================
void MainWindow::setupTab1CircleButton(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // 说明标签
    QLabel *infoLabel = new QLabel(
        tr("演示自定义圆形按钮控件：继承 QWidget，重写 paintEvent 绘制圆形，\n"
           "处理鼠标事件实现悬停/按下/选中状态变化，自定义信号通知外部。"));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888; font-size: 12px; margin: 8px;");
    mainLayout->addWidget(infoLabel);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(30);

    // ---- 普通圆形按钮 ----
    QVBoxLayout *normalGroup = new QVBoxLayout;
    QLabel *normalTitle = new QLabel(tr("普通圆形按钮"));
    normalTitle->setAlignment(Qt::AlignCenter);
    normalTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    mCircleBtn1 = new CircleButton;
    mCircleBtn1->setText(tr("点击"));
    mCircleBtn1->setRadius(40);
    mCircleBtn1->setNormalColor(QColor(70, 130, 180));

    normalGroup->addWidget(normalTitle);
    normalGroup->addWidget(mCircleBtn1, 0, Qt::AlignCenter);
    buttonLayout->addLayout(normalGroup);

    // ---- 可选中按钮组 ----
    QVBoxLayout *checkableGroup = new QVBoxLayout;
    QLabel *checkableTitle = new QLabel(tr("可选中圆形按钮"));
    checkableTitle->setAlignment(Qt::AlignCenter);
    checkableTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *checkBtnsLayout = new QHBoxLayout;
    checkBtnsLayout->setSpacing(20);

    mCircleBtn2 = new CircleButton;
    mCircleBtn2->setText(tr("A"));
    mCircleBtn2->setRadius(30);
    mCircleBtn2->setCheckable(true);
    mCircleBtn2->setNormalColor(QColor(100, 180, 100));

    mCircleBtn3 = new CircleButton;
    mCircleBtn3->setText(tr("B"));
    mCircleBtn3->setRadius(30);
    mCircleBtn3->setCheckable(true);
    mCircleBtn3->setNormalColor(QColor(180, 130, 60));

    checkBtnsLayout->addWidget(mCircleBtn2);
    checkBtnsLayout->addWidget(mCircleBtn3);

    checkableGroup->addWidget(checkableTitle);
    checkableGroup->addLayout(checkBtnsLayout);
    buttonLayout->addLayout(checkableGroup);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(20);

    // 状态显示标签
    mCircleStatusLabel = new QLabel(tr("状态：等待交互..."));
    mCircleStatusLabel->setAlignment(Qt::AlignCenter);
    mCircleStatusLabel->setStyleSheet(
        "background: #2a2a2a; color: #0f0; padding: 10px; "
        "border-radius: 6px; font-size: 13px; font-family: monospace;");
    mainLayout->addWidget(mCircleStatusLabel);

    mainLayout->addStretch();

    // 连接信号
    connect(mCircleBtn1, &CircleButton::clicked, this, [this](bool) {
        mCircleStatusLabel->setText(tr("状态：普通按钮被点击！"));
    });

    connect(mCircleBtn2, &CircleButton::clicked, this, [this](bool checked) {
        mCircleStatusLabel->setText(
            tr("状态：按钮 A %1").arg(checked ? tr("已选中") : tr("取消选中")));
    });

    connect(mCircleBtn3, &CircleButton::clicked, this, [this](bool checked) {
        mCircleStatusLabel->setText(
            tr("状态：按钮 B %1").arg(checked ? tr("已选中") : tr("取消选中")));
    });

    connect(mCircleBtn2, &CircleButton::stateChanged, this, [this](int state) {
        mCircleStatusLabel->setText(
            tr("状态：按钮 A 状态变化 → %1").arg(state));
    });
}

// ============================================================
// Tab 2: 进度仪表盘 —— 仪表盘控件演示
// ============================================================
void MainWindow::setupTab2GaugeWidget(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // 说明标签
    QLabel *infoLabel = new QLabel(
        tr("演示进度仪表盘控件：继承 QWidget 绘制半圆弧仪表盘，\n"
           "包含刻度线、指针、数值显示，支持设置范围和颜色，\n"
           "使用 QTimer 实现平滑的动画效果。"));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888; font-size: 12px; margin: 8px;");
    mainLayout->addWidget(infoLabel);

    // 仪表盘控件
    mGaugeWidget = new GaugeWidget;
    mGaugeWidget->setRange(0, 100);
    mGaugeWidget->setValue(0);

    // 居中放置仪表盘
    QHBoxLayout *gaugeLayout = new QHBoxLayout;
    gaugeLayout->addStretch();
    gaugeLayout->addWidget(mGaugeWidget);
    gaugeLayout->addStretch();
    mainLayout->addLayout(gaugeLayout);

    // 控制面板
    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setSpacing(20);

    // 滑块控制
    QLabel *sliderLabel = new QLabel(tr("手动控制："));
    mGaugeSlider = new QSlider(Qt::Horizontal);
    mGaugeSlider->setRange(0, 100);
    mGaugeSlider->setValue(0);
    mGaugeSlider->setTickPosition(QSlider::TicksBelow);
    mGaugeSlider->setTickInterval(10);

    mGaugeValueLabel = new QLabel(tr("0.0"));
    mGaugeValueLabel->setFixedWidth(50);
    mGaugeValueLabel->setStyleSheet("font-weight: bold; font-size: 16px;");

    controlLayout->addWidget(sliderLabel);
    controlLayout->addWidget(mGaugeSlider, 1);
    controlLayout->addWidget(mGaugeValueLabel);
    mainLayout->addLayout(controlLayout);

    // 按钮面板
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(10);

    QPushButton *animLowBtn = new QPushButton(tr("动画到 25"));
    QPushButton *animMidBtn = new QPushButton(tr("动画到 50"));
    QPushButton *animHighBtn = new QPushButton(tr("动画到 75"));
    QPushButton *animFullBtn = new QPushButton(tr("动画到 100"));
    QPushButton *resetBtn = new QPushButton(tr("重置"));

    btnLayout->addWidget(animLowBtn);
    btnLayout->addWidget(animMidBtn);
    btnLayout->addWidget(animHighBtn);
    btnLayout->addWidget(animFullBtn);
    btnLayout->addWidget(resetBtn);
    mainLayout->addLayout(btnLayout);

    mainLayout->addStretch();

    // 连接信号
    connect(mGaugeSlider, &QSlider::valueChanged, this, [this](int value) {
        mGaugeWidget->setValue(value);
        mGaugeValueLabel->setText(QString::number(value, 'f', 1));
    });

    connect(mGaugeWidget, &GaugeWidget::valueChanged, this, [this](double value) {
        mGaugeValueLabel->setText(QString::number(value, 'f', 1));
    });

    connect(animLowBtn, &QPushButton::clicked, this, [this]() {
        mGaugeWidget->animateToValue(25);
    });
    connect(animMidBtn, &QPushButton::clicked, this, [this]() {
        mGaugeWidget->animateToValue(50);
    });
    connect(animHighBtn, &QPushButton::clicked, this, [this]() {
        mGaugeWidget->animateToValue(75);
    });
    connect(animFullBtn, &QPushButton::clicked, this, [this]() {
        mGaugeWidget->animateToValue(100);
    });
    connect(resetBtn, &QPushButton::clicked, this, [this]() {
        mGaugeWidget->animateToValue(0);
    });
}

// ============================================================
// Tab 3: 自定义 Delegate —— 委托渲染演示
// ============================================================
void MainWindow::setupTab3Delegate(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // 说明标签
    QLabel *infoLabel = new QLabel(
        tr("演示自定义 QStyledItemDelegate：在 QTableView 中实现\n"
           "进度条渲染、星级评分渲染、颜色块渲染，支持编辑功能。\n"
           "点击星级评分列可编辑评分（0-5星）。"));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888; font-size: 12px; margin: 8px;");
    mainLayout->addWidget(infoLabel);

    // 创建表格视图
    mTableView = new QTableView;
    mTableView->setAlternatingRowColors(true);
    mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    // 创建数据模型
    mTableModel = new QStandardItemModel(this);
    mTableModel->setHorizontalHeaderLabels(
        QStringList() << tr("任务名称") << tr("进度") << tr("评分") << tr("状态颜色")
    );

    // 示例数据
    struct TaskData {
        const char *name;
        int progress;
        int stars;
        QColor color;
    };

    TaskData tasks[] = {
        { "用户界面开发",   85, 4, QColor(60, 180, 80)   },
        { "数据库设计",     60, 3, QColor(240, 180, 30)   },
        { "网络通信模块",   35, 2, QColor(220, 60, 60)    },
        { "单元测试",       95, 5, QColor(70, 130, 180)   },
        { "文档编写",       20, 1, QColor(180, 100, 200)  },
        { "性能优化",       50, 3, QColor(100, 200, 150)  },
        { "部署配置",       10, 0, QColor(200, 150, 100)  },
        { "代码审查",       75, 4, QColor(130, 180, 220)  },
    };

    for (int i = 0; i < 8; ++i) {
        QList<QStandardItem *> rowItems;

        // 列 0: 任务名称（默认渲染）
        QStandardItem *nameItem = new QStandardItem(tr(tasks[i].name));
        nameItem->setEditable(false);
        rowItems << nameItem;

        // 列 1: 进度（进度条渲染）
        QStandardItem *progressItem = new QStandardItem;
        progressItem->setData(tasks[i].progress, Qt::DisplayRole);
        progressItem->setData(tasks[i].progress, Qt::EditRole);
        progressItem->setEditable(false);
        rowItems << progressItem;

        // 列 2: 评分（星级渲染）
        QStandardItem *starItem = new QStandardItem;
        starItem->setData(tasks[i].stars, Qt::DisplayRole);
        starItem->setData(tasks[i].stars, Qt::EditRole);
        rowItems << starItem;

        // 列 3: 状态颜色（颜色块渲染）
        QStandardItem *colorItem = new QStandardItem;
        colorItem->setData(tasks[i].color, Qt::DisplayRole);
        colorItem->setEditable(false);
        rowItems << colorItem;

        mTableModel->appendRow(rowItems);
    }

    mTableView->setModel(mTableModel);

    // 调整列宽
    mTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    mTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    mTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    mTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    mTableView->setColumnWidth(1, 150);
    mTableView->setColumnWidth(2, 120);
    mTableView->setColumnWidth(3, 120);

    // 创建并设置自定义委托
    mStarDelegate = new StarDelegate(this);
    mStarDelegate->setColumnType(1, StarDelegate::TypeProgress); // 进度列
    mStarDelegate->setColumnType(2, StarDelegate::TypeStar);       // 星级列
    mStarDelegate->setColumnType(3, StarDelegate::TypeColor);       // 颜色列
    mTableView->setItemDelegate(mStarDelegate);

    mainLayout->addWidget(mTableView);
}

// ============================================================
// Tab 4: 组合控件 —— 文件路径选择器 + 搜索框演示
// ============================================================
void MainWindow::setupTab4CompositeWidget(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // 说明标签
    QLabel *infoLabel = new QLabel(
        tr("演示组合控件：将多个标准 Widget 封装为可复用的复合控件。\n"
           "文件路径选择器 = QLineEdit + 浏览按钮；\n"
           "搜索框 = QLineEdit + 清除按钮，支持回车触发搜索。"));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #888; font-size: 12px; margin: 8px;");
    mainLayout->addWidget(infoLabel);

    // ---- 文件路径选择器组 ----
    QGroupBox *pathGroup = new QGroupBox(tr("文件路径选择器"));
    QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);

    QLabel *pathDesc = new QLabel(
        tr("封装 QLineEdit + QPushButton，提供统一的文件选择接口。\n"
           "支持手动输入路径或点击浏览按钮通过文件对话框选择。"));
    pathDesc->setWordWrap(true);
    pathDesc->setStyleSheet("color: #aaa; font-size: 11px;");
    pathLayout->addWidget(pathDesc);

    mPathSelector = new FilePathSelector;
    mPathSelector->setFilter(
        tr("所有文件 (*);;C++ 文件 (*.cpp *.h);;文本文件 (*.txt);;CMake 文件 (CMakeLists.txt)"));

    QHBoxLayout *pathBtnLayout = new QHBoxLayout;
    QPushButton *setPathBtn = new QPushButton(tr("设置示例路径"));
    QPushButton *clearPathBtn = new QPushButton(tr("清除路径"));
    pathBtnLayout->addWidget(setPathBtn);
    pathBtnLayout->addWidget(clearPathBtn);
    pathBtnLayout->addStretch();

    mPathDisplayLabel = new QLabel(tr("当前路径：（无）"));
    mPathDisplayLabel->setStyleSheet(
        "background: #2a2a2a; color: #0f0; padding: 6px; "
        "border-radius: 4px; font-family: monospace; font-size: 12px;");
    mPathDisplayLabel->setWordWrap(true);

    pathLayout->addWidget(mPathSelector);
    pathLayout->addLayout(pathBtnLayout);
    pathLayout->addWidget(new QLabel(tr("路径变化信号输出：")));
    pathLayout->addWidget(mPathDisplayLabel);

    mainLayout->addWidget(pathGroup);

    // ---- 搜索框组 ----
    QGroupBox *searchGroup = new QGroupBox(tr("搜索框"));
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);

    QLabel *searchDesc = new QLabel(
        tr("封装 QLineEdit + 清除按钮，输入文字时自动显示清除按钮。\n"
           "支持 textChanged 信号、searchRequested 信号和 cleared 信号。"));
    searchDesc->setWordWrap(true);
    searchDesc->setStyleSheet("color: #aaa; font-size: 11px;");
    searchLayout->addWidget(searchDesc);

    mSearchBox = new SearchBox;
    mSearchBox->setPlaceholderText(tr("输入关键词后按回车搜索..."));

    QHBoxLayout *searchInfoLayout = new QHBoxLayout;
    mSearchResultLabel = new QLabel(tr("搜索结果：（无）"));
    mSearchResultLabel->setStyleSheet(
        "background: #2a2a2a; color: #0f0; padding: 6px; "
        "border-radius: 4px; font-family: monospace; font-size: 12px;");
    searchInfoLayout->addWidget(new QLabel(tr("搜索信号输出：")));
    searchInfoLayout->addWidget(mSearchResultLabel, 1);

    searchLayout->addWidget(mSearchBox);
    searchLayout->addLayout(searchInfoLayout);

    mainLayout->addWidget(searchGroup);
    mainLayout->addStretch();

    // 连接信号
    connect(mPathSelector, &FilePathSelector::filePathChanged, this, [this](const QString &path) {
        mPathDisplayLabel->setText(tr("当前路径：%1").arg(path.isEmpty() ? tr("（无）") : path));
    });

    connect(setPathBtn, &QPushButton::clicked, this, [this]() {
        mPathSelector->setFilePath("/home/user/example/custom_widget.cpp");
    });

    connect(clearPathBtn, &QPushButton::clicked, this, [this]() {
        mPathSelector->setFilePath("");
    });

    connect(mSearchBox, &SearchBox::textChanged, this, [this](const QString &text) {
        mSearchResultLabel->setText(
            tr("搜索结果：textChanged(\"%1\")").arg(text));
    });

    connect(mSearchBox, &SearchBox::searchRequested, this, [this](const QString &text) {
        mSearchResultLabel->setText(
            tr("搜索结果：searchRequested(\"%1\") - 触发搜索！").arg(text));
    });

    connect(mSearchBox, &SearchBox::cleared, this, [this]() {
        mSearchResultLabel->setText(tr("搜索结果：cleared 信号 - 已清除！"));
    });
}
