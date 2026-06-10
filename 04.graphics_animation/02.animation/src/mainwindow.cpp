#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QFinalState>
#include <QSignalTransition>
#include <QKeyEventTransition>
#include <QSequentialAnimationGroup>

#include <cmath>

// ============================================================
// 构造 / 析构
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_propBtn(nullptr)
    , m_easingCombo(nullptr)
    , m_directionCombo(nullptr)
    , m_loopCombo(nullptr)
    , m_animTargetCombo(nullptr)
    , m_propStartBtn(nullptr)
    , m_propStopBtn(nullptr)
    , m_propAnim(nullptr)
    , m_seqBtn(nullptr)
    , m_parallelBtn(nullptr)
    , m_groupStartBtn(nullptr)
    , m_groupPauseBtn(nullptr)
    , m_groupResumeBtn(nullptr)
    , m_groupStopBtn(nullptr)
    , m_groupStatusLabel(nullptr)
    , m_seqGroup(nullptr)
    , m_paraGroup(nullptr)
    , m_windowAnimWidget(nullptr)
    , m_opacityEffect(nullptr)
    , m_fadeInBtn(nullptr)
    , m_fadeOutBtn(nullptr)
    , m_moveBtn(nullptr)
    , m_scaleBtn(nullptr)
    , m_windowFadeAnim(nullptr)
    , m_windowMoveAnim(nullptr)
    , m_windowScaleAnim(nullptr)
    , m_stateLabel(nullptr)
    , m_stateBtn(nullptr)
    , m_stateMachine(nullptr)
    , m_redLight(nullptr)
    , m_yellowLight(nullptr)
    , m_greenLight(nullptr)
    , m_trafficStartBtn(nullptr)
    , m_trafficStopBtn(nullptr)
    , m_trafficStatusLabel(nullptr)
    , m_trafficMachine(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
}

// ============================================================
// setupUI - 主界面布局
// ============================================================
void MainWindow::setupUI()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Tab 1: 属性动画
    QWidget *tab1 = new QWidget();
    createPropertyAnimationTab(tab1);
    tabWidget->addTab(tab1, "属性动画");

    // Tab 2: 动画组
    QWidget *tab2 = new QWidget();
    createAnimationGroupTab(tab2);
    tabWidget->addTab(tab2, "动画组");

    // Tab 3: 窗口动画
    QWidget *tab3 = new QWidget();
    createWindowAnimationTab(tab3);
    tabWidget->addTab(tab3, "窗口动画");

    // Tab 4: 状态机
    QWidget *tab4 = new QWidget();
    createStateMachineTab(tab4);
    tabWidget->addTab(tab4, "状态机");

    // Tab 5: 综合示例 - 红绿灯
    QWidget *tab5 = new QWidget();
    createTrafficLightTab(tab5);
    tabWidget->addTab(tab5, "红绿灯演示");
}

// ============================================================
// Tab 1: 属性动画 - QPropertyAnimation 控制按钮属性
// ============================================================
void MainWindow::createPropertyAnimationTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 演示区域 ----
    QGroupBox *demoGroup = new QGroupBox("动画演示区域", parent);
    demoGroup->setMinimumHeight(200);
    QVBoxLayout *demoLayout = new QVBoxLayout(demoGroup);

    m_propBtn = new QPushButton("动画按钮", demoGroup);
    m_propBtn->setFixedSize(120, 50);
    demoLayout->addWidget(m_propBtn, 0, Qt::AlignTop | Qt::AlignLeft);
    demoLayout->addStretch();

    mainLayout->addWidget(demoGroup, 1);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("动画控制", parent);
    QGridLayout *ctrlLayout = new QGridLayout(ctrlGroup);

    // 动画目标属性
    ctrlLayout->addWidget(new QLabel("动画目标:", ctrlGroup), 0, 0);
    m_animTargetCombo = new QComboBox(ctrlGroup);
    m_animTargetCombo->addItems(QStringList()
        << "位置 (pos)" << "大小 (size)" << "透明度 (opacity)");
    ctrlLayout->addWidget(m_animTargetCombo, 0, 1);

    // 缓动曲线
    ctrlLayout->addWidget(new QLabel("缓动曲线:", ctrlGroup), 1, 0);
    m_easingCombo = new QComboBox(ctrlGroup);
    m_easingCombo->addItems(QStringList()
        << "Linear"
        << "InOutCubic"
        << "InOutElastic"
        << "OutBounce"
        << "InOutBack"
        << "InOutQuad"
        << "OutInCirc"
        << "InOutSine");
    ctrlLayout->addWidget(m_easingCombo, 1, 1);

    // 方向
    ctrlLayout->addWidget(new QLabel("动画方向:", ctrlGroup), 2, 0);
    m_directionCombo = new QComboBox(ctrlGroup);
    m_directionCombo->addItems(QStringList()
        << "Forward" << "Backward" << "PingPong");
    ctrlLayout->addWidget(m_directionCombo, 2, 1);

    // 循环
    ctrlLayout->addWidget(new QLabel("循环模式:", ctrlGroup), 3, 0);
    m_loopCombo = new QComboBox(ctrlGroup);
    m_loopCombo->addItems(QStringList()
        << "单次 (Once)" << "循环 (Loop)" << "PingPong循环");
    ctrlLayout->addWidget(m_loopCombo, 3, 1);

    // 按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_propStartBtn = new QPushButton("开始动画", ctrlGroup);
    m_propStopBtn  = new QPushButton("停止动画", ctrlGroup);
    m_propStopBtn->setEnabled(false);
    btnLayout->addWidget(m_propStartBtn);
    btnLayout->addWidget(m_propStopBtn);
    ctrlLayout->addLayout(btnLayout, 4, 0, 1, 2);

    mainLayout->addWidget(ctrlGroup);

    // ---- 创建默认动画 ----
    m_propAnim = new QPropertyAnimation(m_propBtn, "pos", this);
    m_propAnim->setDuration(1000);
    m_propAnim->setStartValue(QPoint(10, 10));
    m_propAnim->setEndValue(QPoint(300, 150));
    m_propAnim->setEasingCurve(QEasingCurve::Linear);

    // ---- 信号连接 ----
    connect(m_propStartBtn, &QPushButton::clicked, this, [this]() {
        // 停止之前的动画
        m_propAnim->stop();

        // 根据目标属性设置动画
        QString target = m_animTargetCombo->currentText();
        if (target.contains("位置")) {
            m_propAnim->setPropertyName("pos");
            m_propAnim->setStartValue(QPoint(10, 10));
            m_propAnim->setEndValue(QPoint(300, 150));
        } else if (target.contains("大小")) {
            m_propAnim->setPropertyName("geometry");
            m_propAnim->setStartValue(QRect(10, 10, 120, 50));
            m_propAnim->setEndValue(QRect(150, 80, 250, 100));
        } else if (target.contains("透明度")) {
            m_propAnim->setPropertyName("opacity");
            m_propAnim->setStartValue(1.0);
            m_propAnim->setEndValue(0.0);
        }

        // 设置缓动曲线
        QString easing = m_easingCombo->currentText();
        if (easing == "Linear")         m_propAnim->setEasingCurve(QEasingCurve::Linear);
        else if (easing == "InOutCubic")    m_propAnim->setEasingCurve(QEasingCurve::InOutCubic);
        else if (easing == "InOutElastic")  m_propAnim->setEasingCurve(QEasingCurve::InOutElastic);
        else if (easing == "OutBounce")     m_propAnim->setEasingCurve(QEasingCurve::OutBounce);
        else if (easing == "InOutBack")     m_propAnim->setEasingCurve(QEasingCurve::InOutBack);
        else if (easing == "InOutQuad")     m_propAnim->setEasingCurve(QEasingCurve::InOutQuad);
        else if (easing == "OutInCirc")     m_propAnim->setEasingCurve(QEasingCurve::OutInCirc);
        else if (easing == "InOutSine")     m_propAnim->setEasingCurve(QEasingCurve::InOutSine);

        // 设置方向 (注意: PingPong 通过循环模式实现, 方向只有 Forward/Backward)
        QString dir = m_directionCombo->currentText();
        if (dir == "Forward")        m_propAnim->setDirection(QAbstractAnimation::Forward);
        else if (dir == "Backward")  m_propAnim->setDirection(QAbstractAnimation::Backward);
        else if (dir == "PingPong")  m_propAnim->setDirection(QAbstractAnimation::Forward); // PingPong方向改为Forward

        // 设置循环
        QString loop = m_loopCombo->currentText();
        if (loop.contains("循环")) {
            m_propAnim->setLoopCount(-1); // 无限循环
        } else {
            m_propAnim->setLoopCount(1);
        }

        m_propAnim->start();
        m_propStartBtn->setEnabled(false);
        m_propStopBtn->setEnabled(true);
    });

    connect(m_propStopBtn, &QPushButton::clicked, this, [this]() {
        m_propAnim->stop();
        m_propStartBtn->setEnabled(true);
        m_propStopBtn->setEnabled(false);
    });

    connect(m_propAnim, &QPropertyAnimation::finished, this, [this]() {
        m_propStartBtn->setEnabled(true);
        m_propStopBtn->setEnabled(false);
    });
}

// ============================================================
// Tab 2: 动画组 - 顺序/并行/嵌套
// ============================================================
void MainWindow::createAnimationGroupTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 演示区域 ----
    QGroupBox *demoGroup = new QGroupBox("动画组演示区域", parent);
    demoGroup->setMinimumHeight(250);
    QVBoxLayout *demoLayout = new QVBoxLayout(demoGroup);

    m_seqBtn = new QPushButton("顺序动画", demoGroup);
    m_seqBtn->setFixedSize(120, 50);
    m_parallelBtn = new QPushButton("并行动画", demoGroup);
    m_parallelBtn->setFixedSize(120, 50);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(m_seqBtn);
    hLayout->addStretch();
    hLayout->addWidget(m_parallelBtn);
    demoLayout->addLayout(hLayout);
    demoLayout->addStretch();

    mainLayout->addWidget(demoGroup, 1);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("控制", parent);
    QVBoxLayout *ctrlLayout = new QVBoxLayout(ctrlGroup);

    m_groupStatusLabel = new QLabel("状态: 就绪", ctrlGroup);
    ctrlLayout->addWidget(m_groupStatusLabel);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_groupStartBtn  = new QPushButton("开始", ctrlGroup);
    m_groupPauseBtn  = new QPushButton("暂停", ctrlGroup);
    m_groupResumeBtn = new QPushButton("恢复", ctrlGroup);
    m_groupStopBtn   = new QPushButton("停止", ctrlGroup);
    m_groupPauseBtn->setEnabled(false);
    m_groupResumeBtn->setEnabled(false);
    m_groupStopBtn->setEnabled(false);
    btnLayout->addWidget(m_groupStartBtn);
    btnLayout->addWidget(m_groupPauseBtn);
    btnLayout->addWidget(m_groupResumeBtn);
    btnLayout->addWidget(m_groupStopBtn);
    ctrlLayout->addLayout(btnLayout);

    QLabel *tipLabel = new QLabel("提示: 顺序动画 - 按钮依次移动到4个位置\n"
                                   "     并行动画 - 按钮同时移动和缩放", ctrlGroup);
    tipLabel->setWordWrap(true);
    ctrlLayout->addWidget(tipLabel);

    mainLayout->addWidget(ctrlGroup);

    // ---- 创建顺序动画组 ----
    m_seqGroup = new QSequentialAnimationGroup(this);
    m_seqGroup->addAnimation(
        createMoveAnimation(m_seqBtn, QPoint(10, 10), QPoint(200, 10)));
    m_seqGroup->addAnimation(
        createMoveAnimation(m_seqBtn, QPoint(200, 10), QPoint(200, 150)));
    m_seqGroup->addAnimation(
        createMoveAnimation(m_seqBtn, QPoint(200, 150), QPoint(10, 150)));
    m_seqGroup->addAnimation(
        createMoveAnimation(m_seqBtn, QPoint(10, 150), QPoint(10, 10)));

    // ---- 创建并行动画组（嵌套在顺序组之后） ----
    m_paraGroup = new QParallelAnimationGroup(this);

    // 并行: 移动
    m_paraGroup->addAnimation(
        createMoveAnimation(m_parallelBtn, QPoint(10, 10), QPoint(300, 150)));
    // 并行: 缩放
    QPropertyAnimation *scaleAnim = new QPropertyAnimation(m_parallelBtn, "geometry");
    scaleAnim->setStartValue(QRect(10, 10, 120, 50));
    scaleAnim->setEndValue(QRect(150, 80, 250, 100));
    scaleAnim->setDuration(1500);
    scaleAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_paraGroup->addAnimation(scaleAnim);

    // ---- 信号连接 ----
    connect(m_groupStartBtn, &QPushButton::clicked, this, [this]() {
        // 重置按钮位置
        m_seqBtn->move(10, 10);
        m_parallelBtn->move(10, 10);
        m_parallelBtn->setGeometry(10, 10, 120, 50);

        m_seqGroup->start();
        m_paraGroup->start();

        m_groupStartBtn->setEnabled(false);
        m_groupPauseBtn->setEnabled(true);
        m_groupStopBtn->setEnabled(true);
        m_groupResumeBtn->setEnabled(false);
        m_groupStatusLabel->setText("状态: 运行中...");
    });

    connect(m_groupPauseBtn, &QPushButton::clicked, this, [this]() {
        m_seqGroup->pause();
        m_paraGroup->pause();
        m_groupPauseBtn->setEnabled(false);
        m_groupResumeBtn->setEnabled(true);
        m_groupStatusLabel->setText("状态: 已暂停");
    });

    connect(m_groupResumeBtn, &QPushButton::clicked, this, [this]() {
        m_seqGroup->resume();
        m_paraGroup->resume();
        m_groupResumeBtn->setEnabled(false);
        m_groupPauseBtn->setEnabled(true);
        m_groupStatusLabel->setText("状态: 运行中...");
    });

    connect(m_groupStopBtn, &QPushButton::clicked, this, [this]() {
        m_seqGroup->stop();
        m_paraGroup->stop();
        m_groupStartBtn->setEnabled(true);
        m_groupPauseBtn->setEnabled(false);
        m_groupResumeBtn->setEnabled(false);
        m_groupStopBtn->setEnabled(false);
        m_groupStatusLabel->setText("状态: 已停止");
    });

    // 任一动画组完成时更新状态
    connect(m_seqGroup, &QSequentialAnimationGroup::finished, this, [this]() {
        m_groupStatusLabel->setText("状态: 顺序动画完成");
        checkGroupFinished();
    });
    connect(m_paraGroup, &QParallelAnimationGroup::finished, this, [this]() {
        m_groupStatusLabel->setText("状态: 并行动画完成");
        checkGroupFinished();
    });
}

// 辅助函数: 创建移动动画
QPropertyAnimation *MainWindow::createMoveAnimation(QWidget *widget, const QPoint &start, const QPoint &end)
{
    QPropertyAnimation *anim = new QPropertyAnimation(widget, "pos");
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    return anim;
}

// 检查所有动画组是否已完成
void MainWindow::checkGroupFinished()
{
    if (m_seqGroup->state() == QAbstractAnimation::Stopped &&
        m_paraGroup->state() == QAbstractAnimation::Stopped) {
        m_groupStartBtn->setEnabled(true);
        m_groupPauseBtn->setEnabled(false);
        m_groupResumeBtn->setEnabled(false);
        m_groupStopBtn->setEnabled(false);
        m_groupStatusLabel->setText("状态: 全部完成");
    }
}

// ============================================================
// Tab 3: 窗口动画 - 淡入淡出/移动/缩放
// ============================================================
void MainWindow::createWindowAnimationTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 演示区域 ----
    QGroupBox *demoGroup = new QGroupBox("窗口动画演示区域", parent);
    demoGroup->setMinimumHeight(200);
    QVBoxLayout *demoLayout = new QVBoxLayout(demoGroup);

    m_windowAnimWidget = new QWidget(demoGroup);
    m_windowAnimWidget->setFixedSize(300, 120);
    m_windowAnimWidget->setStyleSheet("background-color: #3498db; border-radius: 10px;");

    // 透明度效果
    m_opacityEffect = new QGraphicsOpacityEffect(m_windowAnimWidget);
    m_opacityEffect->setOpacity(1.0);
    m_windowAnimWidget->setGraphicsEffect(m_opacityEffect);

    QLabel *innerLabel = new QLabel("窗口动画效果", m_windowAnimWidget);
    innerLabel->setAlignment(Qt::AlignCenter);
    innerLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");

    QHBoxLayout *centerLayout = new QHBoxLayout(m_windowAnimWidget);
    centerLayout->addWidget(innerLabel);

    demoLayout->addWidget(m_windowAnimWidget, 0, Qt::AlignCenter);
    demoLayout->addStretch();

    mainLayout->addWidget(demoGroup, 1);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("动画控制", parent);
    QVBoxLayout *ctrlLayout = new QVBoxLayout(ctrlGroup);

    // 淡入淡出按钮
    QHBoxLayout *fadeLayout = new QHBoxLayout();
    m_fadeInBtn  = new QPushButton("淡入", ctrlGroup);
    m_fadeOutBtn = new QPushButton("淡出", ctrlGroup);
    fadeLayout->addWidget(new QLabel("透明度动画:", ctrlGroup));
    fadeLayout->addWidget(m_fadeInBtn);
    fadeLayout->addWidget(m_fadeOutBtn);
    fadeLayout->addStretch();
    ctrlLayout->addLayout(fadeLayout);

    // 移动/缩放按钮
    QHBoxLayout *moveLayout = new QHBoxLayout();
    m_moveBtn  = new QPushButton("移动动画", ctrlGroup);
    m_scaleBtn = new QPushButton("缩放动画", ctrlGroup);
    moveLayout->addWidget(new QLabel("几何动画:", ctrlGroup));
    moveLayout->addWidget(m_moveBtn);
    moveLayout->addWidget(m_scaleBtn);
    moveLayout->addStretch();
    ctrlLayout->addLayout(moveLayout);

    mainLayout->addWidget(ctrlGroup);

    // ---- 创建动画 ----
    // 淡入淡出动画
    m_windowFadeAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_windowFadeAnim->setDuration(1000);
    m_windowFadeAnim->setStartValue(1.0);
    m_windowFadeAnim->setEndValue(0.0);
    m_windowFadeAnim->setEasingCurve(QEasingCurve::InOutQuad);

    // 移动动画
    m_windowMoveAnim = new QPropertyAnimation(m_windowAnimWidget, "pos", this);
    m_windowMoveAnim->setDuration(800);
    m_windowMoveAnim->setStartValue(QPoint(100, 20));
    m_windowMoveAnim->setEndValue(QPoint(250, 80));
    m_windowMoveAnim->setEasingCurve(QEasingCurve::InOutCubic);

    // 缩放动画
    m_windowScaleAnim = new QPropertyAnimation(m_windowAnimWidget, "geometry", this);
    m_windowScaleAnim->setDuration(800);
    m_windowScaleAnim->setStartValue(QRect(100, 20, 300, 120));
    m_windowScaleAnim->setEndValue(QRect(150, 30, 200, 80));
    m_windowScaleAnim->setEasingCurve(QEasingCurve::InOutElastic);

    // ---- 信号连接 ----
    connect(m_fadeInBtn, &QPushButton::clicked, this, [this]() {
        m_windowFadeAnim->stop();
        m_windowFadeAnim->setStartValue(m_opacityEffect->opacity());
        m_windowFadeAnim->setEndValue(1.0);
        m_windowFadeAnim->start();
    });

    connect(m_fadeOutBtn, &QPushButton::clicked, this, [this]() {
        m_windowFadeAnim->stop();
        m_windowFadeAnim->setStartValue(m_opacityEffect->opacity());
        m_windowFadeAnim->setEndValue(0.0);
        m_windowFadeAnim->start();
    });

    connect(m_moveBtn, &QPushButton::clicked, this, [this]() {
        m_windowMoveAnim->stop();
        m_windowMoveAnim->setStartValue(m_windowAnimWidget->pos());
        // 交替移动位置
        if (m_windowMoveAnim->endValue().toPoint() == QPoint(250, 80)) {
            m_windowMoveAnim->setEndValue(QPoint(100, 20));
        } else {
            m_windowMoveAnim->setEndValue(QPoint(250, 80));
        }
        m_windowMoveAnim->start();
    });

    connect(m_scaleBtn, &QPushButton::clicked, this, [this]() {
        m_windowScaleAnim->stop();
        m_windowScaleAnim->setStartValue(m_windowAnimWidget->geometry());
        // 交替缩放大小
        QRect curEnd = m_windowScaleAnim->endValue().toRect();
        if (curEnd.width() < 250) {
            m_windowScaleAnim->setEndValue(QRect(100, 20, 300, 120));
        } else {
            m_windowScaleAnim->setEndValue(QRect(150, 30, 200, 80));
        }
        m_windowScaleAnim->start();
    });
}

// ============================================================
// Tab 4: 状态机 - QStateMachine 演示
// ============================================================
void MainWindow::createStateMachineTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 演示区域 ----
    QGroupBox *demoGroup = new QGroupBox("状态机演示", parent);
    QVBoxLayout *demoLayout = new QVBoxLayout(demoGroup);

    m_stateLabel = new QLabel("当前状态: 待机", demoGroup);
    m_stateLabel->setAlignment(Qt::AlignCenter);
    m_stateLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; padding: 30px;");
    demoLayout->addWidget(m_stateLabel);

    m_stateBtn = new QPushButton("点击切换状态", demoGroup);
    m_stateBtn->setFixedSize(200, 50);
    demoLayout->addWidget(m_stateBtn, 0, Qt::AlignCenter);

    mainLayout->addWidget(demoGroup);

    // ---- 说明区域 ----
    QGroupBox *infoGroup = new QGroupBox("状态机说明", parent);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);

    QLabel *infoLabel = new QLabel(
        "本示例展示一个简单的三态状态机:\n\n"
        "  状态A (待机) <--点击--> 状态B (运行)\n"
        "  状态B (运行) <--点击--> 状态C (暂停)\n"
        "  状态C (暂停) <--点击--> 状态A (待机)\n\n"
        "点击按钮在三个状态之间循环切换。\n"
        "每个状态进入时会改变标签文本和按钮颜色。\n"
        "这是 QStateMachine + QState + QSignalTransition 的基本用法。", infoGroup);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("font-size: 13px;");
    infoLayout->addWidget(infoLabel);

    mainLayout->addWidget(infoGroup);

    // ---- 创建状态机 ----
    m_stateMachine = new QStateMachine(this);

    // 创建三个状态
    QState *stateIdle = new QState(m_stateMachine);
    QState *stateRun  = new QState(m_stateMachine);
    QState *statePause = new QState(m_stateMachine);
    QFinalState *stateFinal = new QFinalState(m_stateMachine);

    // 设置状态进入时的动作
    stateIdle->assignProperty(m_stateLabel, "text", "当前状态: 待机");
    stateIdle->assignProperty(m_stateLabel, "styleSheet",
        "font-size: 24px; font-weight: bold; color: #2c3e50; padding: 30px; background-color: #ecf0f1; border-radius: 5px;");

    stateRun->assignProperty(m_stateLabel, "text", "当前状态: 运行中");
    stateRun->assignProperty(m_stateLabel, "styleSheet",
        "font-size: 24px; font-weight: bold; color: white; padding: 30px; background-color: #27ae60; border-radius: 5px;");

    statePause->assignProperty(m_stateLabel, "text", "当前状态: 已暂停");
    statePause->assignProperty(m_stateLabel, "styleSheet",
        "font-size: 24px; font-weight: bold; color: white; padding: 30px; background-color: #e74c3c; border-radius: 5px;");

    // 设置初始状态属性
    m_stateLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #2c3e50; padding: 30px; background-color: #ecf0f1; border-radius: 5px;");

    // 添加状态转换
    stateIdle->addTransition(m_stateBtn, &QPushButton::clicked, stateRun);
    stateRun->addTransition(m_stateBtn, &QPushButton::clicked, statePause);
    statePause->addTransition(m_stateBtn, &QPushButton::clicked, stateFinal);

    // 设置初始状态
    m_stateMachine->setInitialState(stateIdle);

    // 启动状态机
    m_stateMachine->start();
}

// ============================================================
// Tab 5: 综合示例 - 红绿灯演示
// ============================================================
void MainWindow::createTrafficLightTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 红绿灯显示区域 ----
    QGroupBox *lightGroup = new QGroupBox("交通灯", parent);
    QVBoxLayout *lightLayout = new QVBoxLayout(lightGroup);

    // 交通灯容器
    QWidget *trafficBox = new QWidget(lightGroup);
    trafficBox->setFixedSize(120, 320);
    trafficBox->setStyleSheet("background-color: #2c3e50; border-radius: 15px;");

    QVBoxLayout *boxLayout = new QVBoxLayout(trafficBox);
    boxLayout->setContentsMargins(20, 15, 20, 15);
    boxLayout->setSpacing(10);

    // 红灯
    m_redLight = new QWidget(trafficBox);
    m_redLight->setFixedSize(80, 80);
    m_redLight->setStyleSheet(
        "background-color: #4a0000; border-radius: 40px; border: 2px solid #555;");
    boxLayout->addWidget(m_redLight, 0, Qt::AlignCenter);

    // 黄灯
    m_yellowLight = new QWidget(trafficBox);
    m_yellowLight->setFixedSize(80, 80);
    m_yellowLight->setStyleSheet(
        "background-color: #4a4a00; border-radius: 40px; border: 2px solid #555;");
    boxLayout->addWidget(m_yellowLight, 0, Qt::AlignCenter);

    // 绿灯
    m_greenLight = new QWidget(trafficBox);
    m_greenLight->setFixedSize(80, 80);
    m_greenLight->setStyleSheet(
        "background-color: #004a00; border-radius: 40px; border: 2px solid #555;");
    boxLayout->addWidget(m_greenLight, 0, Qt::AlignCenter);

    lightLayout->addWidget(trafficBox, 0, Qt::AlignCenter);

    // 状态标签
    m_trafficStatusLabel = new QLabel("状态: 停止", lightGroup);
    m_trafficStatusLabel->setAlignment(Qt::AlignCenter);
    m_trafficStatusLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
    lightLayout->addWidget(m_trafficStatusLabel);

    mainLayout->addWidget(lightGroup, 0, Qt::AlignHCenter);

    // ---- 控制按钮 ----
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_trafficStartBtn = new QPushButton("启动交通灯", parent);
    m_trafficStopBtn  = new QPushButton("停止交通灯", parent);
    m_trafficStopBtn->setEnabled(false);
    btnLayout->addWidget(m_trafficStartBtn);
    btnLayout->addWidget(m_trafficStopBtn);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // ---- 说明 ----
    QGroupBox *infoGroup = new QGroupBox("说明", parent);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    QLabel *infoLabel = new QLabel(
        "本示例使用 QStateMachine + 定时器驱动实现红绿灯切换:\n\n"
        "  红灯(5秒) -> 绿灯(5秒) -> 黄灯(2秒) -> 红灯...\n\n"
        "状态转换使用 QTimer 作为触发源,\n"
        "进入每个状态时通过 assignProperty 改变灯的颜色。", infoGroup);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);
    mainLayout->addWidget(infoGroup);
    mainLayout->addStretch();

    // ---- 创建交通灯状态机 ----
    m_trafficMachine = new QStateMachine(this);

    QTimer *redTimer   = new QTimer(this);
    QTimer *greenTimer = new QTimer(this);
    QTimer *yellowTimer = new QTimer(this);
    redTimer->setSingleShot(true);
    greenTimer->setSingleShot(true);
    yellowTimer->setSingleShot(true);

    // 创建三个状态
    QState *redState   = new QState(m_trafficMachine);
    QState *greenState = new QState(m_trafficMachine);
    QState *yellowState = new QState(m_trafficMachine);

    // 红灯状态
    redState->assignProperty(m_trafficStatusLabel, "text", "状态: 停止（红灯）");
    redState->assignProperty(m_trafficStatusLabel, "styleSheet",
        "font-size: 20px; font-weight: bold; margin: 10px; color: #e74c3c;");
    redState->assignProperty(m_redLight, "styleSheet",
        "background-color: #ff0000; border-radius: 40px; border: 2px solid #555;");
    redState->assignProperty(m_yellowLight, "styleSheet",
        "background-color: #4a4a00; border-radius: 40px; border: 2px solid #555;");
    redState->assignProperty(m_greenLight, "styleSheet",
        "background-color: #004a00; border-radius: 40px; border: 2px solid #555;");

    // 绿灯状态
    greenState->assignProperty(m_trafficStatusLabel, "text", "状态: 通行（绿灯）");
    greenState->assignProperty(m_trafficStatusLabel, "styleSheet",
        "font-size: 20px; font-weight: bold; margin: 10px; color: #27ae60;");
    greenState->assignProperty(m_redLight, "styleSheet",
        "background-color: #4a0000; border-radius: 40px; border: 2px solid #555;");
    greenState->assignProperty(m_yellowLight, "styleSheet",
        "background-color: #4a4a00; border-radius: 40px; border: 2px solid #555;");
    greenState->assignProperty(m_greenLight, "styleSheet",
        "background-color: #00ff00; border-radius: 40px; border: 2px solid #555;");

    // 黄灯状态
    yellowState->assignProperty(m_trafficStatusLabel, "text", "状态: 注意（黄灯）");
    yellowState->assignProperty(m_trafficStatusLabel, "styleSheet",
        "font-size: 20px; font-weight: bold; margin: 10px; color: #f39c12;");
    yellowState->assignProperty(m_redLight, "styleSheet",
        "background-color: #4a0000; border-radius: 40px; border: 2px solid #555;");
    yellowState->assignProperty(m_yellowLight, "styleSheet",
        "background-color: #ffff00; border-radius: 40px; border: 2px solid #555;");
    yellowState->assignProperty(m_greenLight, "styleSheet",
        "background-color: #004a00; border-radius: 40px; border: 2px solid #555;");

    // 进入状态时启动对应定时器
    QObject::connect(redState, &QState::entered, this, [redTimer]() { redTimer->start(5000); });
    QObject::connect(greenState, &QState::entered, this, [greenTimer]() { greenTimer->start(5000); });
    QObject::connect(yellowState, &QState::entered, this, [yellowTimer]() { yellowTimer->start(2000); });

    // 添加状态转换（定时器触发）
    redState->addTransition(redTimer, &QTimer::timeout, greenState);
    greenState->addTransition(greenTimer, &QTimer::timeout, yellowState);
    yellowState->addTransition(yellowTimer, &QTimer::timeout, redState);

    // 设置初始状态
    m_trafficMachine->setInitialState(redState);

    // ---- 信号连接 ----
    connect(m_trafficStartBtn, &QPushButton::clicked, this, [this]() {
        m_trafficMachine->start();
        m_trafficStartBtn->setEnabled(false);
        m_trafficStopBtn->setEnabled(true);
    });

    connect(m_trafficStopBtn, &QPushButton::clicked, this, [this]() {
        m_trafficMachine->stop();
        // 停止所有定时器
        foreach (QTimer *timer, this->findChildren<QTimer *>()) {
            timer->stop();
        }
        m_trafficStartBtn->setEnabled(true);
        m_trafficStopBtn->setEnabled(false);
    });
}

// 注意: 需要在头文件中添加 createMoveAnimation 和 checkGroupFinished 声明
// 这里直接在 cpp 中实现了辅助方法
