#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QElapsedTimer>

#include <QtConcurrent/QtConcurrent>
#include <QtConcurrentMap>

#include <QImage>
#include <QPixmap>
#include <QPainter>

#include <cmath>
#include <functional>

// ============================================================
// SimpleTask 实现 - 自定义 QRunnable
// ============================================================
SimpleTask::SimpleTask(int id, QObject *receiver, const char *slot)
    : QRunnable()
    , m_id(id)
    , m_receiver(receiver)
    , m_slot(slot)
{
    setAutoDelete(true);
}

void SimpleTask::run()
{
    // 模拟耗时操作（使用 sleep）
    QThread::msleep(500 + (m_id % 5) * 200);

    QString result = QString("任务 %1 完成").arg(m_id);
    QMetaObject::invokeMethod(m_receiver, m_slot,
        Qt::QueuedConnection,
        Q_ARG(int, m_id),
        Q_ARG(QString, result));
}

// ============================================================
// 构造 / 析构
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_runDurationSpin(nullptr)
    , m_runStepSpin(nullptr)
    , m_runStartBtn(nullptr)
    , m_runCancelBtn(nullptr)
    , m_runProgress(nullptr)
    , m_runLog(nullptr)
    , m_runWatcher(nullptr)
    , m_runCancelFlag(false)
    , m_taskCountSpin(nullptr)
    , m_maxThreadSpin(nullptr)
    , m_threadPoolStartBtn(nullptr)
    , m_threadPoolClearBtn(nullptr)
    , m_threadPoolLog(nullptr)
    , m_completedTaskCount(0)
    , m_imageCountSpin(nullptr)
    , m_imageSizeSpin(nullptr)
    , m_singleThreadBtn(nullptr)
    , m_multiThreadBtn(nullptr)
    , m_imageLog(nullptr)
    , m_imageProgress(nullptr)
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

    // Tab 1: QtConcurrent::run
    QWidget *tab1 = new QWidget();
    createConcurrentRunTab(tab1);
    tabWidget->addTab(tab1, "QtConcurrent::run");

    // Tab 2: map/filter/reduce
    QWidget *tab2 = new QWidget();
    createMapFilterReduceTab(tab2);
    tabWidget->addTab(tab2, "map/filter/reduce");

    // Tab 3: QThreadPool
    QWidget *tab3 = new QWidget();
    createThreadPoolTab(tab3);
    tabWidget->addTab(tab3, "QThreadPool");

    // Tab 4: 并发图像处理
    QWidget *tab4 = new QWidget();
    createImageProcessingTab(tab4);
    tabWidget->addTab(tab4, "并发图像处理");
}

// ============================================================
// Tab 1: QtConcurrent::run - 后台执行耗时任务
// ============================================================
void MainWindow::createConcurrentRunTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("任务参数", parent);
    QGridLayout *ctrlLayout = new QGridLayout(ctrlGroup);

    ctrlLayout->addWidget(new QLabel("总步数:", ctrlGroup), 0, 0);
    m_runStepSpin = new QSpinBox(ctrlGroup);
    m_runStepSpin->setRange(1, 100);
    m_runStepSpin->setValue(20);
    ctrlLayout->addWidget(m_runStepSpin, 0, 1);

    ctrlLayout->addWidget(new QLabel("每步耗时(ms):", ctrlGroup), 1, 0);
    m_runDurationSpin = new QSpinBox(ctrlGroup);
    m_runDurationSpin->setRange(10, 2000);
    m_runDurationSpin->setValue(200);
    m_runDurationSpin->setSingleStep(50);
    ctrlLayout->addWidget(m_runDurationSpin, 1, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_runStartBtn = new QPushButton("开始任务", ctrlGroup);
    m_runCancelBtn = new QPushButton("取消任务", ctrlGroup);
    m_runCancelBtn->setEnabled(false);
    btnLayout->addWidget(m_runStartBtn);
    btnLayout->addWidget(m_runCancelBtn);
    ctrlLayout->addLayout(btnLayout, 2, 0, 1, 2);

    mainLayout->addWidget(ctrlGroup);

    // ---- 进度条 ----
    m_runProgress = new QProgressBar(parent);
    m_runProgress->setRange(0, 100);
    m_runProgress->setValue(0);
    mainLayout->addWidget(m_runProgress);

    // ---- 日志区域 ----
    QGroupBox *logGroup = new QGroupBox("任务日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_runLog = new QTextEdit(logGroup);
    m_runLog->setReadOnly(true);
    logLayout->addWidget(m_runLog);
    mainLayout->addWidget(logGroup, 1);

    // ---- 创建 FutureWatcher ----
    m_runWatcher = new QFutureWatcher<void>(this);

    // ---- 信号连接 ----
    connect(m_runStartBtn, &QPushButton::clicked, this, &MainWindow::onRunStart);
    connect(m_runCancelBtn, &QPushButton::clicked, this, &MainWindow::onRunCanceled);
    connect(m_runWatcher, &QFutureWatcher<void>::finished, this, &MainWindow::onRunFinished);
}

void MainWindow::onRunStart()
{
    int totalSteps = m_runStepSpin->value();
    int durationMs = m_runDurationSpin->value();
    m_runCancelFlag = false;
    m_runProgress->setValue(0);
    m_runLog->clear();
    m_runLog->append(QString("开始任务: 共 %1 步, 每步 %2ms").arg(totalSteps).arg(durationMs));

    m_runStartBtn->setEnabled(false);
    m_runCancelBtn->setEnabled(true);

    int step = totalSteps;
    int ms = durationMs;
    bool *cancelFlag = &m_runCancelFlag;
    QProgressBar *progress = m_runProgress;
    QTextEdit *log = m_runLog;

    // 在后台线程执行耗时任务，通过 QMetaObject 回报进度
    m_runFuture = QtConcurrent::run([step, ms, cancelFlag, progress, log, this]() {
        for (int i = 1; i <= step; ++i) {
            if (*cancelFlag) break;

            // 模拟耗时计算
            QThread::msleep(ms);

            // 通过信号槽报告进度（回到主线程）
            int percent = static_cast<int>(static_cast<double>(i) / step * 100.0);
            QMetaObject::invokeMethod(this, "onRunProgress",
                Qt::QueuedConnection, Q_ARG(int, percent));

            QMetaObject::invokeMethod(log, "append",
                Qt::QueuedConnection,
                Q_ARG(QString, QString("  步骤 %1/%2 完成").arg(i).arg(step)));
        }
    });

    m_runWatcher->setFuture(m_runFuture);
}

void MainWindow::onRunProgress(int value)
{
    m_runProgress->setValue(value);
}

void MainWindow::onRunFinished()
{
    m_runLog->append(m_runCancelFlag ? "任务已取消!" : "任务完成!");
    m_runStartBtn->setEnabled(true);
    m_runCancelBtn->setEnabled(false);
}

void MainWindow::onRunCanceled()
{
    m_runCancelFlag = true;
    m_runLog->append("正在取消任务...");
}

// ============================================================
// Tab 2: map/filter/reduce - 并行数据处理
// ============================================================
void MainWindow::createMapFilterReduceTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 数据源 ----
    QGroupBox *dataGroup = new QGroupBox("数据源", parent);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataGroup);

    QLabel *dataLabel = new QLabel(
        "原始数据: 1 ~ 100 的整数列表\n"
        "使用 QtConcurrent::mapped/filtered/reduced 进行并行处理", dataGroup);
    dataLabel->setWordWrap(true);
    dataLayout->addWidget(dataLabel);

    QHBoxLayout *dataBtnLayout = new QHBoxLayout();
    QPushButton *showDataBtn = new QPushButton("显示原始数据", dataGroup);
    dataBtnLayout->addWidget(showDataBtn);
    dataBtnLayout->addStretch();
    dataLayout->addLayout(dataBtnLayout);

    mainLayout->addWidget(dataGroup);

    // ---- 操作面板 ----
    QGroupBox *opGroup = new QGroupBox("并行操作", parent);
    QVBoxLayout *opLayout = new QVBoxLayout(opGroup);
    opLayout->setSpacing(10);

    QPushButton *mapBtn = new QPushButton("mapped: 每个数乘以3", opGroup);
    QPushButton *filterBtn = new QPushButton("filtered: 筛选偶数", opGroup);
    QPushButton *reduceBtn = new QPushButton("reduced: 求总和", opGroup);
    QPushButton *combinedBtn = new QPushButton("组合: 偶数 * 3 后求和", opGroup);

    opLayout->addWidget(mapBtn);
    opLayout->addWidget(filterBtn);
    opLayout->addWidget(reduceBtn);
    opLayout->addWidget(combinedBtn);

    mainLayout->addWidget(opGroup);

    // ---- 结果区域 ----
    QGroupBox *resultGroup = new QGroupBox("处理结果", parent);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    QTextEdit *resultText = new QTextEdit(resultGroup);
    resultText->setReadOnly(true);
    resultLayout->addWidget(resultText);
    mainLayout->addWidget(resultGroup, 1);

    // ---- 准备数据 ----
    QList<int> dataList;
    for (int i = 1; i <= 100; ++i) {
        dataList << i;
    }

    // ---- 信号连接 ----
    connect(showDataBtn, &QPushButton::clicked, this, [dataList, resultText]() {
        QStringList items;
        for (int v : dataList) {
            items << QString::number(v);
        }
        resultText->append("=== 原始数据 (前20项) ===");
        resultText->append(items.mid(0, 20).join(", ") + ", ...");
        resultText->append(QString("共 %1 个数据").arg(dataList.size()));
    });

    // QtConcurrent::mapped: 并行转换
    connect(mapBtn, &QPushButton::clicked, this, [dataList, resultText]() {
        resultText->append("=== mapped: 每个数乘以3 ===");

        // 使用 std::function 包装 lambda，使 Qt5.13 + C++11 能正确推导返回类型
        std::function<int(const int &)> mapFunc = [](const int &val) -> int {
            QThread::msleep(1); // 模拟计算开销
            return val * 3;
        };
        QFuture<int> future = QtConcurrent::mapped(dataList, mapFunc);

        future.waitForFinished();
        QList<int> results = future.results();

        QStringList items;
        for (int v : results.mid(0, 20)) {
            items << QString::number(v);
        }
        resultText->append("结果 (前20项): " + items.join(", ") + ", ...");
        resultText->append(QString("共 %1 个结果").arg(results.size()));
    });

    // QtConcurrent::filtered: 并行过滤
    connect(filterBtn, &QPushButton::clicked, this, [dataList, resultText]() {
        resultText->append("=== filtered: 筛选偶数 ===");

        std::function<bool(const int &)> filterFunc = [](const int &val) -> bool {
            QThread::msleep(1);
            return (val % 2 == 0);
        };
        QFuture<int> future = QtConcurrent::filtered(dataList, filterFunc);

        future.waitForFinished();
        QList<int> results = future.results();

        QStringList items;
        for (int v : results.mid(0, 20)) {
            items << QString::number(v);
        }
        resultText->append("结果 (前20项): " + items.join(", ") + ", ...");
        resultText->append(QString("共 %1 个偶数").arg(results.size()));
    });

    // QtConcurrent::mappedReduced: 并行映射+聚合
    connect(reduceBtn, &QPushButton::clicked, this, [dataList, resultText]() {
        resultText->append("=== reduced: 求总和 ===");

        std::function<int(const int &)> mapFunc = [](const int &val) -> int {
            QThread::msleep(1);
            return val;
        };
        std::function<void(int &, const int &)> reduceFunc = [](int &sum, const int &val) {
            sum += val;
        };
        int sum = QtConcurrent::mappedReduced<int>(dataList, mapFunc, reduceFunc);
        resultText->append(QString("总和 = %1").arg(sum));
    });

    // 组合操作: 过滤偶数 -> 乘以3 -> 求和
    connect(combinedBtn, &QPushButton::clicked, this, [dataList, resultText]() {
        resultText->append("=== 组合: 偶数 * 3 后求和 ===");

        QElapsedTimer timer;
        timer.start();

        // 先过滤偶数
        std::function<bool(const int &)> filterFunc = [](const int &val) -> bool {
            return (val % 2 == 0);
        };
        QFuture<int> filtered = QtConcurrent::filtered(dataList, filterFunc);
        filtered.waitForFinished();
        QList<int> evenNumbers = filtered.results();

        // 再乘以3
        std::function<int(const int &)> tripleFunc = [](const int &val) -> int {
            return val * 3;
        };
        QFuture<int> mapped = QtConcurrent::mapped(evenNumbers, tripleFunc);
        mapped.waitForFinished();
        QList<int> tripled = mapped.results();

        // 最后求和
        int sum = 0;
        for (int v : tripled) {
            sum += v;
        }

        qint64 elapsed = timer.elapsed();
        resultText->append(QString("偶数个数: %1").arg(evenNumbers.size()));
        resultText->append(QString("结果总和: %1").arg(sum));
        resultText->append(QString("并行处理耗时: %1 ms").arg(elapsed));
    });
}

// ============================================================
// Tab 3: QThreadPool - 线程池管理
// ============================================================
void MainWindow::createThreadPoolTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("线程池配置", parent);
    QGridLayout *ctrlLayout = new QGridLayout(ctrlGroup);

    ctrlLayout->addWidget(new QLabel("任务数量:", ctrlGroup), 0, 0);
    m_taskCountSpin = new QSpinBox(ctrlGroup);
    m_taskCountSpin->setRange(1, 50);
    m_taskCountSpin->setValue(10);
    ctrlLayout->addWidget(m_taskCountSpin, 0, 1);

    ctrlLayout->addWidget(new QLabel("最大线程数:", ctrlGroup), 1, 0);
    m_maxThreadSpin = new QSpinBox(ctrlGroup);
    m_maxThreadSpin->setRange(1, 16);
    m_maxThreadSpin->setValue(4);
    ctrlLayout->addWidget(m_maxThreadSpin, 1, 1);

    QLabel *infoLabel = new QLabel(
        "提示: 每个任务模拟 500~1400ms 的耗时操作\n"
        "观察多线程并发执行时的完成顺序", ctrlGroup);
    infoLabel->setWordWrap(true);
    ctrlLayout->addWidget(infoLabel, 2, 0, 1, 2);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_threadPoolStartBtn = new QPushButton("提交任务", ctrlGroup);
    m_threadPoolClearBtn = new QPushButton("清空日志", ctrlGroup);
    btnLayout->addWidget(m_threadPoolStartBtn);
    btnLayout->addWidget(m_threadPoolClearBtn);
    ctrlLayout->addLayout(btnLayout, 3, 0, 1, 2);

    mainLayout->addWidget(ctrlGroup);

    // ---- 线程池信息 ----
    QGroupBox *poolInfoGroup = new QGroupBox("线程池信息", parent);
    QVBoxLayout *poolInfoLayout = new QVBoxLayout(poolInfoGroup);

    QLabel *poolInfoLabel = new QLabel(poolInfoGroup);
    QThreadPool *pool = QThreadPool::globalInstance();
    poolInfoLabel->setText(QString(
        "全局线程池: maxThreadCount = %1\n"
        "activeThreadCount = %2")
        .arg(pool->maxThreadCount())
        .arg(pool->activeThreadCount()));
    poolInfoLayout->addWidget(poolInfoLabel);

    mainLayout->addWidget(poolInfoGroup);

    // ---- 日志区域 ----
    QGroupBox *logGroup = new QGroupBox("任务日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_threadPoolLog = new QTextEdit(logGroup);
    m_threadPoolLog->setReadOnly(true);
    logLayout->addWidget(m_threadPoolLog);
    mainLayout->addWidget(logGroup, 1);

    // ---- 信号连接 ----
    connect(m_threadPoolStartBtn, &QPushButton::clicked, this, &MainWindow::onThreadPoolStart);
    connect(m_threadPoolClearBtn, &QPushButton::clicked, this, &MainWindow::onThreadPoolClear);
}

void MainWindow::onThreadPoolStart()
{
    int taskCount = m_taskCountSpin->value();
    int maxThreads = m_maxThreadSpin->value();

    // 设置线程池最大线程数
    QThreadPool::globalInstance()->setMaxThreadCount(maxThreads);

    m_completedTaskCount = 0;
    m_threadPoolStartBtn->setEnabled(false);

    m_threadPoolLog->append(QString("=== 提交 %1 个任务, 最大线程数: %2 ===")
        .arg(taskCount).arg(maxThreads));
    m_threadPoolLog->append(QString("当前活跃线程: %1")
        .arg(QThreadPool::globalInstance()->activeThreadCount()));

    // 提交任务到线程池
    for (int i = 0; i < taskCount; ++i) {
        SimpleTask *task = new SimpleTask(i, this, "onTaskFinished");

        // 根据索引设置不同的优先级 (Qt5: 0=普通, >0=高, <0=低)
        int priority = 0;
        QString priorityStr = "普通";
        if (i % 3 == 0) {
            priority = 1;
            priorityStr = "高";
        } else if (i % 3 == 2) {
            priority = -1;
            priorityStr = "低";
        }

        m_threadPoolLog->append(QString("  提交任务 %1 (优先级: %2)")
            .arg(i).arg(priorityStr));

        QThreadPool::globalInstance()->start(task, priority);
    }
}

void MainWindow::onTaskFinished(int taskId, const QString &result)
{
    m_threadPoolLog->append(QString("  [%1] %2").arg(taskId).arg(result));

    m_completedTaskCount++;
    int total = m_taskCountSpin->value();

    if (m_completedTaskCount >= total) {
        m_threadPoolLog->append(QString("=== 全部 %1 个任务已完成 ===").arg(total));
        m_threadPoolStartBtn->setEnabled(true);
    }
}

void MainWindow::onThreadPoolClear()
{
    m_threadPoolLog->clear();
}

// ============================================================
// Tab 4: 并发图像处理 - 单线程 vs 多线程对比
// ============================================================
void MainWindow::createImageProcessingTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("图像处理参数", parent);
    QGridLayout *ctrlLayout = new QGridLayout(ctrlGroup);

    ctrlLayout->addWidget(new QLabel("图片数量:", ctrlGroup), 0, 0);
    m_imageCountSpin = new QSpinBox(ctrlGroup);
    m_imageCountSpin->setRange(1, 100);
    m_imageCountSpin->setValue(20);
    ctrlLayout->addWidget(m_imageCountSpin, 0, 1);

    ctrlLayout->addWidget(new QLabel("图片尺寸(px):", ctrlGroup), 1, 0);
    m_imageSizeSpin = new QSpinBox(ctrlGroup);
    m_imageSizeSpin->setRange(10, 2000);
    m_imageSizeSpin->setValue(200);
    m_imageSizeSpin->setSingleStep(50);
    ctrlLayout->addWidget(m_imageSizeSpin, 1, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_singleThreadBtn = new QPushButton("单线程处理", ctrlGroup);
    m_multiThreadBtn  = new QPushButton("多线程处理", ctrlGroup);
    btnLayout->addWidget(m_singleThreadBtn);
    btnLayout->addWidget(m_multiThreadBtn);
    ctrlLayout->addLayout(btnLayout, 2, 0, 1, 2);

    mainLayout->addWidget(ctrlGroup);

    // ---- 进度 ----
    m_imageProgress = new QProgressBar(parent);
    m_imageProgress->setRange(0, 100);
    m_imageProgress->setValue(0);
    mainLayout->addWidget(m_imageProgress);

    // ---- 日志 ----
    QGroupBox *logGroup = new QGroupBox("处理日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_imageLog = new QTextEdit(logGroup);
    m_imageLog->setReadOnly(true);
    logLayout->addWidget(m_imageLog);
    mainLayout->addWidget(logGroup, 1);

    // ---- 信号连接 ----
    connect(m_singleThreadBtn, &QPushButton::clicked, this, [this]() {
        int count = m_imageCountSpin->value();
        int size = m_imageSizeSpin->value();
        m_imageLog->append(QString("=== 单线程处理 %1 张 %2x%2 图片 ===").arg(count).arg(size));

        QElapsedTimer timer;
        timer.start();

        // 生成原始图片列表
        QList<QImage> images;
        for (int i = 0; i < count; ++i) {
            QImage img(size, size, QImage::Format_RGB32);
            img.fill(qRgb(i * 25 % 256, (i * 37) % 256, (i * 53) % 256));
            images << img;
        }

        // 单线程处理: 缩放到一半大小
        int halfSize = size / 2;
        QList<QImage> results;
        for (int i = 0; i < images.size(); ++i) {
            results << images[i].scaled(halfSize, halfSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QThread::msleep(5); // 模拟处理耗时

            int percent = static_cast<int>((i + 1) * 100.0 / count);
            m_imageProgress->setValue(percent);
            QApplication::processEvents(); // 保持UI响应
        }

        qint64 elapsed = timer.elapsed();
        m_imageLog->append(QString("处理完成: %1 张图片").arg(results.size()));
        m_imageLog->append(QString("单线程耗时: %1 ms").arg(elapsed));
        m_imageLog->append(QString("平均每张: %1 ms").arg(elapsed / count));
    });

    connect(m_multiThreadBtn, &QPushButton::clicked, this, [this]() {
        int count = m_imageCountSpin->value();
        int size = m_imageSizeSpin->value();
        m_imageLog->append(QString("=== 多线程处理 %1 张 %2x%2 图片 ===").arg(count).arg(size));
        m_imageProgress->setValue(0);

        QElapsedTimer timer;
        timer.start();

        // 生成原始图片列表
        QList<QImage> images;
        for (int i = 0; i < count; ++i) {
            QImage img(size, size, QImage::Format_RGB32);
            img.fill(qRgb(i * 25 % 256, (i * 37) % 256, (i * 53) % 256));
            images << img;
        }

        int halfSize = size / 2;

        // 多线程处理: 使用 QtConcurrent::mapped
        std::function<QImage(const QImage &)> scaleFunc = [halfSize](const QImage &img) -> QImage {
            QThread::msleep(5); // 模拟处理耗时
            return img.scaled(halfSize, halfSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        };
        QFuture<QImage> future = QtConcurrent::mapped(images, scaleFunc);

        // 监听进度
        QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>(this);
        connect(watcher, &QFutureWatcher<QImage>::progressValueChanged, this,
            [this](int value) {
                int total = m_imageCountSpin->value();
                int percent = static_cast<int>(value * 100.0 / total);
                m_imageProgress->setValue(percent);
            });

        connect(watcher, &QFutureWatcher<QImage>::finished, this,
            [this, watcher, timer, count]() {
                qint64 elapsed = timer.elapsed();
                QList<QImage> results = watcher->future().results();
                m_imageLog->append(QString("处理完成: %1 张图片").arg(results.size()));
                m_imageLog->append(QString("多线程耗时: %1 ms").arg(elapsed));
                m_imageLog->append(QString("平均每张: %1 ms").arg(elapsed / count));
                watcher->deleteLater();
            });

        watcher->setFuture(future);
    });
}
