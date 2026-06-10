#include "mainwindow.h"
#include "worker.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_timer(nullptr)
    , m_remainingSeconds(10)
    , m_timerProgressBar(nullptr)
    , m_timerLabel(nullptr)
    , m_timerStartBtn(nullptr)
    , m_timerStopBtn(nullptr)
    , m_timerResetBtn(nullptr)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
    , m_workProgressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_startWorkBtn(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    cleanupThread();
}

void MainWindow::setupUI()
{
    setWindowTitle("Demo 08 - Timer & Thread");
    resize(420, 500);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // === Timer Section ===
    QGroupBox *timerGroup = new QGroupBox("Timer Section", centralWidget);
    QVBoxLayout *timerLayout = new QVBoxLayout(timerGroup);

    m_timerLabel = new QLabel("Remaining: 10 s", timerGroup);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_timerLabel->font();
    font.setPointSize(16);
    m_timerLabel->setFont(font);
    timerLayout->addWidget(m_timerLabel);

    m_timerProgressBar = new QProgressBar(timerGroup);
    m_timerProgressBar->setRange(0, 10);
    m_timerProgressBar->setValue(10);
    m_timerProgressBar->setFormat("%v seconds");
    timerLayout->addWidget(m_timerProgressBar);

    QHBoxLayout *timerBtnLayout = new QHBoxLayout();
    m_timerStartBtn = new QPushButton("Start", timerGroup);
    m_timerStopBtn = new QPushButton("Stop", timerGroup);
    m_timerResetBtn = new QPushButton("Reset", timerGroup);
    m_timerStopBtn->setEnabled(false);
    timerBtnLayout->addWidget(m_timerStartBtn);
    timerBtnLayout->addWidget(m_timerStopBtn);
    timerBtnLayout->addWidget(m_timerResetBtn);
    timerLayout->addLayout(timerBtnLayout);

    mainLayout->addWidget(timerGroup);

    connect(m_timerStartBtn, &QPushButton::clicked, this, &MainWindow::onTimerStart);
    connect(m_timerStopBtn, &QPushButton::clicked, this, &MainWindow::onTimerStop);
    connect(m_timerResetBtn, &QPushButton::clicked, this, &MainWindow::onTimerReset);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);

    // === Thread Section ===
    QGroupBox *threadGroup = new QGroupBox("Thread Section", centralWidget);
    QVBoxLayout *threadLayout = new QVBoxLayout(threadGroup);

    m_statusLabel = new QLabel("Status: Idle", threadGroup);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    threadLayout->addWidget(m_statusLabel);

    m_workProgressBar = new QProgressBar(threadGroup);
    m_workProgressBar->setRange(0, 100);
    m_workProgressBar->setValue(0);
    threadLayout->addWidget(m_workProgressBar);

    QHBoxLayout *threadBtnLayout = new QHBoxLayout();
    m_startWorkBtn = new QPushButton("Start Work", threadGroup);
    threadBtnLayout->addWidget(m_startWorkBtn);
    threadLayout->addLayout(threadBtnLayout);

    mainLayout->addWidget(threadGroup);

    connect(m_startWorkBtn, &QPushButton::clicked, this, &MainWindow::onStartWork);
}

void MainWindow::cleanupThread()
{
    if (m_workerThread) {
        if (m_workerThread->isRunning()) {
            m_workerThread->quit();
            m_workerThread->wait(2000);
        }
        delete m_workerThread;
        m_workerThread = nullptr;
    }
    delete m_worker;
    m_worker = nullptr;
}

void MainWindow::onTimerStart()
{
    m_timer->start();
    m_timerStartBtn->setEnabled(false);
    m_timerStopBtn->setEnabled(true);
    m_timerResetBtn->setEnabled(false);
}

void MainWindow::onTimerStop()
{
    m_timer->stop();
    m_timerStartBtn->setEnabled(true);
    m_timerStopBtn->setEnabled(false);
    m_timerResetBtn->setEnabled(true);
}

void MainWindow::onTimerReset()
{
    m_timer->stop();
    m_remainingSeconds = 10;
    m_timerProgressBar->setValue(10);
    m_timerLabel->setText("Remaining: 10 s");
    m_timerStartBtn->setEnabled(true);
    m_timerStopBtn->setEnabled(false);
    m_timerResetBtn->setEnabled(false);
}

void MainWindow::onTimerTick()
{
    --m_remainingSeconds;
    m_timerProgressBar->setValue(m_remainingSeconds);
    m_timerLabel->setText(QString("Remaining: %1 s").arg(m_remainingSeconds));

    if (m_remainingSeconds <= 0) {
        m_timer->stop();
        m_timerLabel->setText("Time is up!");
        m_timerStartBtn->setEnabled(false);
        m_timerStopBtn->setEnabled(false);
        m_timerResetBtn->setEnabled(true);
    }
}

void MainWindow::onStartWork()
{
    if (m_workerThread && m_workerThread->isRunning()) {
        return;
    }

    cleanupThread();

    m_worker = new Worker(nullptr);
    m_workerThread = new QThread(this);

    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(this, &MainWindow::destroyed, m_workerThread, &QThread::quit);
    connect(m_worker, &Worker::progress, this, &MainWindow::onWorkProgress);
    connect(m_worker, &Worker::finished, this, &MainWindow::onWorkFinished);

    m_workProgressBar->setValue(0);
    m_statusLabel->setText("Status: Working...");
    m_startWorkBtn->setEnabled(false);

    m_workerThread->start();

    QMetaObject::invokeMethod(m_worker, "doWork", Qt::QueuedConnection, Q_ARG(int, 100));
}

void MainWindow::onWorkProgress(int value)
{
    m_workProgressBar->setValue(value);
}

void MainWindow::onWorkFinished()
{
    m_statusLabel->setText("Status: Done");
    m_startWorkBtn->setEnabled(true);
    m_workProgressBar->setValue(100);

    m_workerThread->quit();
    m_workerThread->wait(2000);
}
