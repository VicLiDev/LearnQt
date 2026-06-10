#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QSpinBox>
#include <QProgressBar>
#include <QFuture>
#include <QFutureWatcher>
#include <QRunnable>
#include <QThreadPool>

// 自定义 Runnable 任务，用于线程池演示
class SimpleTask : public QRunnable
{
public:
    explicit SimpleTask(int id, QObject *receiver, const char *slot);
    void run() override;

private:
    int m_id;
    QObject *m_receiver;
    const char *m_slot;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Tab 1: QtConcurrent::run
    void onRunStart();
    void onRunFinished();
    void onRunCanceled();
    void onRunProgress(int value);

    // Tab 3: QThreadPool
    void onThreadPoolStart();
    void onTaskFinished(int taskId, const QString &result);
    void onThreadPoolClear();

private:
    void setupUI();

    // === Tab 1: QtConcurrent::run ===
    void createConcurrentRunTab(QWidget *parent);

    // === Tab 2: map/filter/reduce ===
    void createMapFilterReduceTab(QWidget *parent);

    // === Tab 3: QThreadPool ===
    void createThreadPoolTab(QWidget *parent);

    // === Tab 4: 并发图像处理 ===
    void createImageProcessingTab(QWidget *parent);

    // Tab1 控件
    QSpinBox    *m_runDurationSpin;
    QSpinBox    *m_runStepSpin;
    QPushButton *m_runStartBtn;
    QPushButton *m_runCancelBtn;
    QProgressBar *m_runProgress;
    QTextEdit   *m_runLog;
    QFuture<void> m_runFuture;
    QFutureWatcher<void> *m_runWatcher;
    bool m_runCancelFlag;

    // Tab3 控件
    QSpinBox    *m_taskCountSpin;
    QSpinBox    *m_maxThreadSpin;
    QPushButton *m_threadPoolStartBtn;
    QPushButton *m_threadPoolClearBtn;
    QTextEdit   *m_threadPoolLog;
    int m_completedTaskCount;

    // Tab4 控件
    QSpinBox    *m_imageCountSpin;
    QSpinBox    *m_imageSizeSpin;
    QPushButton *m_singleThreadBtn;
    QPushButton *m_multiThreadBtn;
    QTextEdit   *m_imageLog;
    QProgressBar *m_imageProgress;
};

#endif // MAINWINDOW_H
