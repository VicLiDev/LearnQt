#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QThread>

class Worker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTimerStart();
    void onTimerStop();
    void onTimerReset();
    void onTimerTick();

    void onStartWork();
    void onWorkProgress(int value);
    void onWorkFinished();

private:
    void setupUI();
    void cleanupThread();

    // Timer section
    QTimer *m_timer;
    int m_remainingSeconds;
    QProgressBar *m_timerProgressBar;
    QLabel *m_timerLabel;
    QPushButton *m_timerStartBtn;
    QPushButton *m_timerStopBtn;
    QPushButton *m_timerResetBtn;

    // Thread section
    QThread *m_workerThread;
    Worker *m_worker;
    QProgressBar *m_workProgressBar;
    QLabel *m_statusLabel;
    QPushButton *m_startWorkBtn;
};

#endif // MAINWINDOW_H
