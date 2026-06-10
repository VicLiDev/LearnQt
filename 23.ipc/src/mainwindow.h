#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QSpinBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Tab 1: 共享内存
    void onCreateSharedMemory();
    void onDetachSharedMemory();
    void onWriteSharedMemory();
    void onReadSharedMemory();

    // Tab 2: 本地套接字
    void onServerStart();
    void onServerStop();
    void onClientConnect();
    void onClientDisconnect();
    void onClientSend();
    void onServerNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();
    void onServerError(QLocalSocket::LocalSocketError error);

    // Tab 3: 进程管理
    void onProcessStart();
    void onProcessWrite();
    void onProcessStop();
    void readProcessOutput();
    void readProcessError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

    // Tab 4: 综合示例
    void onIpcServerStart();
    void onIpcClientConnect();
    void onIpcSendMessage();

private:
    void setupUI();

    // === Tab 1: 共享内存 ===
    void createSharedMemoryTab(QWidget *parent);

    // === Tab 2: 本地套接字 ===
    void createLocalSocketTab(QWidget *parent);

    // === Tab 3: 进程管理 ===
    void createProcessTab(QWidget *parent);

    // === Tab 4: 综合示例 ===
    void createIpcExampleTab(QWidget *parent);

    // Tab1 控件
    QLineEdit *m_shmKeyEdit;
    QSpinBox  *m_shmSizeSpin;
    QLineEdit *m_shmWriteEdit;
    QLabel    *m_shmStatusLabel;
    QTextEdit *m_shmReadArea;
    QTextEdit *m_shmLog;
    QPushButton *m_shmCreateBtn;
    QPushButton *m_shmDetachBtn;
    QPushButton *m_shmWriteBtn;
    QPushButton *m_shmReadBtn;
    QSharedMemory *m_sharedMemory;

    // Tab2 控件
    QLineEdit   *m_serverNameEdit;
    QLabel      *m_serverStatusLabel;
    QTextEdit   *m_serverLog;
    QPushButton *m_serverStartBtn;
    QPushButton *m_serverStopBtn;
    QLocalServer *m_localServer;

    QLineEdit   *m_clientServerEdit;
    QLineEdit   *m_clientSendEdit;
    QTextEdit   *m_clientLog;
    QLabel      *m_clientStatusLabel;
    QPushButton *m_clientConnectBtn;
    QPushButton *m_clientDisconnectBtn;
    QPushButton *m_clientSendBtn;
    QLocalSocket *m_clientSocket;

    // Tab3 控件
    QLineEdit   *m_processCmdEdit;
    QLineEdit   *m_processInputEdit;
    QTextEdit   *m_processOutput;
    QTextEdit   *m_processLog;
    QLabel      *m_processStatusLabel;
    QPushButton *m_processStartBtn;
    QPushButton *m_processStopBtn;
    QPushButton *m_processWriteBtn;
    QProcess    *m_process;

    // Tab4 控件
    QLineEdit   *m_ipcServerNameEdit;
    QLineEdit   *m_ipcClientMsgEdit;
    QTextEdit   *m_ipcLog;
    QLabel      *m_ipcStatusLabel;
    QPushButton *m_ipcServerStartBtn;
    QPushButton *m_ipcClientConnectBtn;
    QPushButton *m_ipcSendBtn;
    QLocalServer *m_ipcServer;
    QLocalSocket *m_ipcClient;
};

#endif // MAINWINDOW_H
