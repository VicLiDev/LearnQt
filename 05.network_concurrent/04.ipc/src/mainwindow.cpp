#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTime>
#include <QMessageBox>

// ============================================================
// 构造 / 析构
// ============================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_shmKeyEdit(nullptr)
    , m_shmSizeSpin(nullptr)
    , m_shmWriteEdit(nullptr)
    , m_shmStatusLabel(nullptr)
    , m_shmReadArea(nullptr)
    , m_shmLog(nullptr)
    , m_shmCreateBtn(nullptr)
    , m_shmDetachBtn(nullptr)
    , m_shmWriteBtn(nullptr)
    , m_shmReadBtn(nullptr)
    , m_sharedMemory(nullptr)
    , m_serverNameEdit(nullptr)
    , m_serverStatusLabel(nullptr)
    , m_serverLog(nullptr)
    , m_serverStartBtn(nullptr)
    , m_serverStopBtn(nullptr)
    , m_localServer(nullptr)
    , m_clientServerEdit(nullptr)
    , m_clientSendEdit(nullptr)
    , m_clientLog(nullptr)
    , m_clientStatusLabel(nullptr)
    , m_clientConnectBtn(nullptr)
    , m_clientDisconnectBtn(nullptr)
    , m_clientSendBtn(nullptr)
    , m_clientSocket(nullptr)
    , m_processCmdEdit(nullptr)
    , m_processInputEdit(nullptr)
    , m_processOutput(nullptr)
    , m_processLog(nullptr)
    , m_processStatusLabel(nullptr)
    , m_processStartBtn(nullptr)
    , m_processStopBtn(nullptr)
    , m_processWriteBtn(nullptr)
    , m_process(nullptr)
    , m_ipcServerNameEdit(nullptr)
    , m_ipcClientMsgEdit(nullptr)
    , m_ipcLog(nullptr)
    , m_ipcStatusLabel(nullptr)
    , m_ipcServerStartBtn(nullptr)
    , m_ipcClientConnectBtn(nullptr)
    , m_ipcSendBtn(nullptr)
    , m_ipcServer(nullptr)
    , m_ipcClient(nullptr)
{
    setupUI();
}

MainWindow::~MainWindow()
{
    // 清理共享内存
    if (m_sharedMemory) {
        if (m_sharedMemory->isAttached()) {
            m_sharedMemory->detach();
        }
        delete m_sharedMemory;
    }

    // 清理本地套接字服务端
    if (m_localServer) {
        m_localServer->close();
        delete m_localServer;
    }

    // 清理本地套接字客户端
    if (m_clientSocket) {
        m_clientSocket->disconnectFromServer();
        delete m_clientSocket;
    }

    // 清理进程
    if (m_process) {
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
        delete m_process;
    }

    // 清理IPC服务端
    if (m_ipcServer) {
        m_ipcServer->close();
        delete m_ipcServer;
    }

    // 清理IPC客户端
    if (m_ipcClient) {
        m_ipcClient->disconnectFromServer();
        delete m_ipcClient;
    }
}

// ============================================================
// setupUI - 主界面布局
// ============================================================
void MainWindow::setupUI()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Tab 1: 共享内存
    QWidget *tab1 = new QWidget();
    createSharedMemoryTab(tab1);
    tabWidget->addTab(tab1, "共享内存");

    // Tab 2: 本地套接字
    QWidget *tab2 = new QWidget();
    createLocalSocketTab(tab2);
    tabWidget->addTab(tab2, "本地套接字");

    // Tab 3: 进程管理
    QWidget *tab3 = new QWidget();
    createProcessTab(tab3);
    tabWidget->addTab(tab3, "进程管理");

    // Tab 4: 综合示例
    QWidget *tab4 = new QWidget();
    createIpcExampleTab(tab4);
    tabWidget->addTab(tab4, "综合示例");
}

// ============================================================
// Tab 1: 共享内存 - QSharedMemory
// ============================================================
void MainWindow::createSharedMemoryTab(QWidget *parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(parent);

    // ---- 左侧: 写入端 ----
    QGroupBox *writeGroup = new QGroupBox("写入端", parent);
    QVBoxLayout *writeLayout = new QVBoxLayout(writeGroup);

    // 共享内存配置
    QGroupBox *configGroup = new QGroupBox("配置", writeGroup);
    QGridLayout *configLayout = new QGridLayout(configGroup);

    configLayout->addWidget(new QLabel("Key:", configGroup), 0, 0);
    m_shmKeyEdit = new QLineEdit("QT_DEMO_SHM", configGroup);
    configLayout->addWidget(m_shmKeyEdit, 0, 1);

    configLayout->addWidget(new QLabel("大小(字节):", configGroup), 1, 0);
    m_shmSizeSpin = new QSpinBox(configGroup);
    m_shmSizeSpin->setRange(64, 65536);
    m_shmSizeSpin->setValue(1024);
    m_shmSizeSpin->setSingleStep(256);
    configLayout->addWidget(m_shmSizeSpin, 1, 1);

    writeLayout->addWidget(configGroup);

    // 状态
    m_shmStatusLabel = new QLabel("状态: 未创建", writeGroup);
    writeLayout->addWidget(m_shmStatusLabel);

    // 写入内容
    writeLayout->addWidget(new QLabel("写入数据:", writeGroup));
    m_shmWriteEdit = new QLineEdit(writeGroup);
    m_shmWriteEdit->setPlaceholderText("输入要写入共享内存的文本");
    writeLayout->addWidget(m_shmWriteEdit);

    // 按钮
    QHBoxLayout *writeBtnLayout = new QHBoxLayout();
    m_shmCreateBtn = new QPushButton("创建", writeGroup);
    m_shmWriteBtn  = new QPushButton("写入", writeGroup);
    m_shmDetachBtn = new QPushButton("分离", writeGroup);
    writeBtnLayout->addWidget(m_shmCreateBtn);
    writeBtnLayout->addWidget(m_shmWriteBtn);
    writeBtnLayout->addWidget(m_shmDetachBtn);
    writeLayout->addLayout(writeBtnLayout);

    // 读取区域
    writeLayout->addWidget(new QLabel("读取到的数据:", writeGroup));
    m_shmReadArea = new QTextEdit(writeGroup);
    m_shmReadArea->setReadOnly(true);
    m_shmReadArea->setMaximumHeight(100);
    writeLayout->addWidget(m_shmReadArea);

    m_shmReadBtn = new QPushButton("读取", writeGroup);
    writeLayout->addWidget(m_shmReadBtn);

    mainLayout->addWidget(writeGroup);

    // ---- 右侧: 日志 ----
    QGroupBox *logGroup = new QGroupBox("操作日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_shmLog = new QTextEdit(logGroup);
    m_shmLog->setReadOnly(true);
    logLayout->addWidget(m_shmLog);
    mainLayout->addWidget(logGroup, 1);

    // ---- 创建共享内存对象 ----
    m_sharedMemory = new QSharedMemory(this);

    // ---- 信号连接 ----
    connect(m_shmCreateBtn, &QPushButton::clicked, this, &MainWindow::onCreateSharedMemory);
    connect(m_shmWriteBtn, &QPushButton::clicked, this, &MainWindow::onWriteSharedMemory);
    connect(m_shmReadBtn, &QPushButton::clicked, this, &MainWindow::onReadSharedMemory);
    connect(m_shmDetachBtn, &QPushButton::clicked, this, &MainWindow::onDetachSharedMemory);
}

void MainWindow::onCreateSharedMemory()
{
    QString key = m_shmKeyEdit->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入共享内存 Key");
        return;
    }

    // 先分离已有的
    if (m_sharedMemory->isAttached()) {
        m_sharedMemory->detach();
    }

    m_sharedMemory->setKey(key);

    if (m_sharedMemory->create(m_shmSizeSpin->value())) {
        m_shmStatusLabel->setText(QString("状态: 已创建 (key=%1, size=%2)")
            .arg(key).arg(m_shmSizeSpin->value()));
        m_shmLog->append(QString("[%1] 共享内存创建成功, key=%2, 大小=%3 字节")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(key).arg(m_shmSizeSpin->value()));
    } else {
        // 可能已存在，尝试附加
        if (m_sharedMemory->attach()) {
            m_shmStatusLabel->setText(QString("状态: 已附加到已有的共享内存 (key=%1)").arg(key));
            m_shmLog->append(QString("[%1] 共享内存已存在, 成功附加 (key=%2)")
                .arg(QTime::currentTime().toString("HH:mm:ss")).arg(key));
        } else {
            m_shmStatusLabel->setText("状态: 创建/附加失败");
            m_shmLog->append(QString("[%1] 错误: %2")
                .arg(QTime::currentTime().toString("HH:mm:ss"))
                .arg(m_sharedMemory->errorString()));
        }
    }
}

void MainWindow::onWriteSharedMemory()
{
    if (!m_sharedMemory->isAttached()) {
        m_shmLog->append("请先创建或附加共享内存");
        return;
    }

    QString text = m_shmWriteEdit->text();
    if (text.isEmpty()) {
        m_shmLog->append("请输入要写入的数据");
        return;
    }

    // 分离后重新创建以确保大小足够
    QByteArray data = text.toUtf8();
    if (data.size() > m_sharedMemory->size()) {
        m_shmLog->append(QString("数据大小(%1)超过共享内存大小(%2), 尝试重新创建")
            .arg(data.size()).arg(m_sharedMemory->size()));

        m_sharedMemory->detach();
        m_sharedMemory->setKey(m_shmKeyEdit->text().trimmed());
        if (!m_sharedMemory->create(data.size() + 64)) {
            m_shmLog->append("重新创建失败: " + m_sharedMemory->errorString());
            return;
        }
    }

    // 写入数据
    m_sharedMemory->lock();
    char *to = static_cast<char *>(m_sharedMemory->data());
    memset(to, 0, m_sharedMemory->size());
    memcpy(to, data.constData(), qMin(data.size(), m_sharedMemory->size()));
    m_sharedMemory->unlock();

    m_shmLog->append(QString("[%1] 写入数据: \"%2\" (%3 字节)")
        .arg(QTime::currentTime().toString("HH:mm:ss"))
        .arg(text).arg(data.size()));
}

void MainWindow::onReadSharedMemory()
{
    // 先尝试附加（如果尚未附加）
    if (!m_sharedMemory->isAttached()) {
        m_sharedMemory->setKey(m_shmKeyEdit->text().trimmed());
        if (!m_sharedMemory->attach()) {
            m_shmLog->append("无法附加到共享内存: " + m_sharedMemory->errorString());
            return;
        }
        m_shmLog->append(QString("成功附加到共享内存 (key=%1)").arg(m_shmKeyEdit->text()));
    }

    // 读取数据
    m_sharedMemory->lock();
    char *from = static_cast<char *>(m_sharedMemory->data());
    QByteArray data(from, m_sharedMemory->size());
    m_sharedMemory->unlock();

    // 转换为字符串，去除尾部的空字符
    QString text = QString::fromUtf8(data).trimmed();
    m_shmReadArea->setPlainText(text);

    m_shmLog->append(QString("[%1] 读取数据: \"%2\"")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(text));
}

void MainWindow::onDetachSharedMemory()
{
    if (m_sharedMemory->isAttached()) {
        m_sharedMemory->detach();
        m_shmStatusLabel->setText("状态: 已分离");
        m_shmLog->append(QString("[%1] 已从共享内存分离")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
    } else {
        m_shmLog->append("当前未附加到共享内存");
    }
}

// ============================================================
// Tab 2: 本地套接字 - QLocalServer / QLocalSocket
// ============================================================
void MainWindow::createLocalSocketTab(QWidget *parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(parent);

    // ---- 左侧: 服务端 ----
    QGroupBox *serverGroup = new QGroupBox("服务端 (QLocalServer)", parent);
    QVBoxLayout *serverLayout = new QVBoxLayout(serverGroup);

    serverLayout->addWidget(new QLabel("服务名称:", serverGroup));
    m_serverNameEdit = new QLineEdit("qt_demo_local_server", serverGroup);
    serverLayout->addWidget(m_serverNameEdit);

    m_serverStatusLabel = new QLabel("状态: 未启动", serverGroup);
    m_serverStatusLabel->setStyleSheet("font-weight: bold;");
    serverLayout->addWidget(m_serverStatusLabel);

    QHBoxLayout *serverBtnLayout = new QHBoxLayout();
    m_serverStartBtn = new QPushButton("启动服务", serverGroup);
    m_serverStopBtn  = new QPushButton("停止服务", serverGroup);
    m_serverStopBtn->setEnabled(false);
    serverBtnLayout->addWidget(m_serverStartBtn);
    serverBtnLayout->addWidget(m_serverStopBtn);
    serverLayout->addLayout(serverBtnLayout);

    serverLayout->addWidget(new QLabel("消息日志:", serverGroup));
    m_serverLog = new QTextEdit(serverGroup);
    m_serverLog->setReadOnly(true);
    serverLayout->addWidget(m_serverLog);

    mainLayout->addWidget(serverGroup);

    // ---- 右侧: 客户端 ----
    QGroupBox *clientGroup = new QGroupBox("客户端 (QLocalSocket)", parent);
    QVBoxLayout *clientLayout = new QVBoxLayout(clientGroup);

    clientLayout->addWidget(new QLabel("连接服务:", clientGroup));
    m_clientServerEdit = new QLineEdit("qt_demo_local_server", clientGroup);
    clientLayout->addWidget(m_clientServerEdit);

    m_clientStatusLabel = new QLabel("状态: 未连接", clientGroup);
    m_clientStatusLabel->setStyleSheet("font-weight: bold;");
    clientLayout->addWidget(m_clientStatusLabel);

    QHBoxLayout *clientConnLayout = new QHBoxLayout();
    m_clientConnectBtn    = new QPushButton("连接", clientGroup);
    m_clientDisconnectBtn = new QPushButton("断开", clientGroup);
    m_clientDisconnectBtn->setEnabled(false);
    clientConnLayout->addWidget(m_clientConnectBtn);
    clientConnLayout->addWidget(m_clientDisconnectBtn);
    clientLayout->addLayout(clientConnLayout);

    clientLayout->addWidget(new QLabel("发送消息:", clientGroup));
    m_clientSendEdit = new QLineEdit(clientGroup);
    m_clientSendEdit->setPlaceholderText("输入要发送的消息");
    clientLayout->addWidget(m_clientSendEdit);

    m_clientSendBtn = new QPushButton("发送", clientGroup);
    m_clientSendBtn->setEnabled(false);
    clientLayout->addWidget(m_clientSendBtn);

    clientLayout->addWidget(new QLabel("消息日志:", clientGroup));
    m_clientLog = new QTextEdit(clientGroup);
    m_clientLog->setReadOnly(true);
    clientLayout->addWidget(m_clientLog);

    mainLayout->addWidget(clientGroup);

    // ---- 创建服务端和客户端 ----
    m_localServer = new QLocalServer(this);
    m_clientSocket = new QLocalSocket(this);

    // ---- 服务端信号连接 ----
    connect(m_serverStartBtn, &QPushButton::clicked, this, &MainWindow::onServerStart);
    connect(m_serverStopBtn, &QPushButton::clicked, this, &MainWindow::onServerStop);
    connect(m_localServer, &QLocalServer::newConnection, this, &MainWindow::onServerNewConnection);

    // ---- 客户端信号连接 ----
    connect(m_clientConnectBtn, &QPushButton::clicked, this, &MainWindow::onClientConnect);
    connect(m_clientDisconnectBtn, &QPushButton::clicked, this, &MainWindow::onClientDisconnect);
    connect(m_clientSendBtn, &QPushButton::clicked, this, &MainWindow::onClientSend);
    connect(m_clientSocket, &QLocalSocket::readyRead, this, &MainWindow::onClientReadyRead);
    connect(m_clientSocket, &QLocalSocket::disconnected, this, &MainWindow::onClientDisconnected);
    connect(m_clientSocket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error),
        this, &MainWindow::onServerError);
}

void MainWindow::onServerStart()
{
    QString serverName = m_serverNameEdit->text().trimmed();
    if (serverName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入服务名称");
        return;
    }

    // 移除可能残留的旧服务
    QLocalServer::removeServer(serverName);

    if (m_localServer->listen(serverName)) {
        m_serverStatusLabel->setText(QString("状态: 监听中 (%1)").arg(serverName));
        m_serverStatusLabel->setStyleSheet("font-weight: bold; color: green;");
        m_serverLog->append(QString("[%1] 服务启动成功, 监听: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss")).arg(serverName));
        m_serverStartBtn->setEnabled(false);
        m_serverStopBtn->setEnabled(true);
    } else {
        m_serverLog->append(QString("[%1] 服务启动失败: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(m_localServer->errorString()));
    }
}

void MainWindow::onServerStop()
{
    // 断开所有客户端连接
    foreach (QLocalSocket *socket, m_localServer->findChildren<QLocalSocket *>()) {
        socket->disconnectFromServer();
    }

    m_localServer->close();
    m_serverStatusLabel->setText("状态: 已停止");
    m_serverStatusLabel->setStyleSheet("font-weight: bold; color: red;");
    m_serverLog->append(QString("[%1] 服务已停止")
        .arg(QTime::currentTime().toString("HH:mm:ss")));
    m_serverStartBtn->setEnabled(true);
    m_serverStopBtn->setEnabled(false);
}

void MainWindow::onServerNewConnection()
{
    QLocalSocket *clientSocket = m_localServer->nextPendingConnection();
    m_serverLog->append(QString("[%1] 新客户端连接")
        .arg(QTime::currentTime().toString("HH:mm:ss")));

    // 读取客户端发来的数据
    connect(clientSocket, &QLocalSocket::readyRead, this, [this, clientSocket]() {
        QByteArray data = clientSocket->readAll();
        m_serverLog->append(QString("[%1] 收到: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(QString::fromUtf8(data)));

        // 回复客户端
        QString reply = QString("服务器收到: %1").arg(QString::fromUtf8(data));
        clientSocket->write(reply.toUtf8());
        clientSocket->flush();
    });

    // 客户端断开
    connect(clientSocket, &QLocalSocket::disconnected, this, [this, clientSocket]() {
        m_serverLog->append(QString("[%1] 客户端断开")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
        clientSocket->deleteLater();
    });
}

void MainWindow::onClientConnect()
{
    QString serverName = m_clientServerEdit->text().trimmed();
    if (serverName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入要连接的服务名称");
        return;
    }

    m_clientStatusLabel->setText("状态: 正在连接...");
    m_clientStatusLabel->setStyleSheet("font-weight: bold; color: orange;");

    m_clientSocket->connectToServer(serverName);

    m_clientConnectBtn->setEnabled(false);
    m_clientDisconnectBtn->setEnabled(true);
    m_clientSendBtn->setEnabled(true);
}

void MainWindow::onClientDisconnect()
{
    m_clientSocket->disconnectFromServer();
    m_clientStatusLabel->setText("状态: 已断开");
    m_clientStatusLabel->setStyleSheet("font-weight: bold; color: red;");
    m_clientLog->append(QString("[%1] 已断开连接")
        .arg(QTime::currentTime().toString("HH:mm:ss")));
    m_clientConnectBtn->setEnabled(true);
    m_clientDisconnectBtn->setEnabled(false);
    m_clientSendBtn->setEnabled(false);
}

void MainWindow::onClientSend()
{
    QString msg = m_clientSendEdit->text().trimmed();
    if (msg.isEmpty()) return;

    m_clientSocket->write(msg.toUtf8());
    m_clientSocket->flush();
    m_clientLog->append(QString("[%1] 发送: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(msg));
    m_clientSendEdit->clear();
}

void MainWindow::onClientReadyRead()
{
    QByteArray data = m_clientSocket->readAll();
    m_clientLog->append(QString("[%1] 收到: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss"))
        .arg(QString::fromUtf8(data)));

    // 连接成功后首次收到数据，更新状态
    if (m_clientStatusLabel->text().contains("正在连接")) {
        m_clientStatusLabel->setText("状态: 已连接");
        m_clientStatusLabel->setStyleSheet("font-weight: bold; color: green;");
    }
}

void MainWindow::onClientDisconnected()
{
    m_clientStatusLabel->setText("状态: 已断开");
    m_clientStatusLabel->setStyleSheet("font-weight: bold; color: red;");
    m_clientLog->append(QString("[%1] 连接已断开")
        .arg(QTime::currentTime().toString("HH:mm:ss")));
    m_clientConnectBtn->setEnabled(true);
    m_clientDisconnectBtn->setEnabled(false);
    m_clientSendBtn->setEnabled(false);
}

void MainWindow::onServerError(QLocalSocket::LocalSocketError error)
{
    Q_UNUSED(error);
    m_clientLog->append(QString("[%1] 错误: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss"))
        .arg(m_clientSocket->errorString()));

    if (m_clientSocket->state() != QLocalSocket::ConnectedState) {
        m_clientStatusLabel->setText("状态: 连接失败");
        m_clientStatusLabel->setStyleSheet("font-weight: bold; color: red;");
        m_clientConnectBtn->setEnabled(true);
        m_clientDisconnectBtn->setEnabled(false);
        m_clientSendBtn->setEnabled(false);
    }
}

// ============================================================
// Tab 3: 进程管理 - QProcess
// ============================================================
void MainWindow::createProcessTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 命令输入 ----
    QGroupBox *cmdGroup = new QGroupBox("命令配置", parent);
    QVBoxLayout *cmdLayout = new QVBoxLayout(cmdGroup);

    QHBoxLayout *cmdInputLayout = new QHBoxLayout();
    cmdInputLayout->addWidget(new QLabel("命令:", cmdGroup));
    m_processCmdEdit = new QLineEdit(cmdGroup);
    m_processCmdEdit->setPlaceholderText("例如: ls -la, grep --help, cat /etc/hostname");
    cmdInputLayout->addWidget(m_processCmdEdit);
    cmdLayout->addLayout(cmdInputLayout);

    // 预设命令按钮
    QHBoxLayout *presetLayout = new QHBoxLayout();
    QPushButton *presetLs = new QPushButton("ls -la", cmdGroup);
    QPushButton *presetDate = new QPushButton("date", cmdGroup);
    QPushButton *presetUname = new QPushButton("uname -a", cmdGroup);
    QPushButton *presetPwd = new QPushButton("pwd", cmdGroup);
    presetLayout->addWidget(presetLs);
    presetLayout->addWidget(presetDate);
    presetLayout->addWidget(presetUname);
    presetLayout->addWidget(presetPwd);
    cmdLayout->addLayout(presetLayout);

    // 输入
    QHBoxLayout *stdinLayout = new QHBoxLayout();
    stdinLayout->addWidget(new QLabel("标准输入:", cmdGroup));
    m_processInputEdit = new QLineEdit(cmdGroup);
    m_processInputEdit->setPlaceholderText("向进程标准输入写入数据");
    stdinLayout->addWidget(m_processInputEdit);
    m_processWriteBtn = new QPushButton("写入 stdin", cmdGroup);
    m_processWriteBtn->setEnabled(false);
    stdinLayout->addWidget(m_processWriteBtn);
    cmdLayout->addLayout(stdinLayout);

    // 操作按钮
    QHBoxLayout *opLayout = new QHBoxLayout();
    m_processStartBtn = new QPushButton("启动进程", cmdGroup);
    m_processStopBtn  = new QPushButton("终止进程", cmdGroup);
    m_processStopBtn->setEnabled(false);
    opLayout->addWidget(m_processStartBtn);
    opLayout->addWidget(m_processStopBtn);
    opLayout->addStretch();
    cmdLayout->addLayout(opLayout);

    mainLayout->addWidget(cmdGroup);

    // ---- 状态 ----
    m_processStatusLabel = new QLabel("状态: 就绪", parent);
    m_processStatusLabel->setStyleSheet("font-weight: bold; padding: 5px;");
    mainLayout->addWidget(m_processStatusLabel);

    // ---- 输出区域 ----
    QHBoxLayout *outputLayout = new QHBoxLayout();

    QGroupBox *stdoutGroup = new QGroupBox("标准输出 (stdout)", parent);
    QVBoxLayout *stdoutLayout = new QVBoxLayout(stdoutGroup);
    m_processOutput = new QTextEdit(stdoutGroup);
    m_processOutput->setReadOnly(true);
    stdoutLayout->addWidget(m_processOutput);
    outputLayout->addWidget(stdoutGroup);

    QGroupBox *logGroup = new QGroupBox("日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_processLog = new QTextEdit(logGroup);
    m_processLog->setReadOnly(true);
    logLayout->addWidget(m_processLog);
    outputLayout->addWidget(logGroup);

    mainLayout->addLayout(outputLayout, 1);

    // ---- 创建进程 ----
    m_process = new QProcess(this);

    // ---- 信号连接 ----
    connect(m_processStartBtn, &QPushButton::clicked, this, &MainWindow::onProcessStart);
    connect(m_processStopBtn, &QPushButton::clicked, this, &MainWindow::onProcessStop);
    connect(m_processWriteBtn, &QPushButton::clicked, this, &MainWindow::onProcessWrite);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &MainWindow::readProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &MainWindow::readProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainWindow::onProcessFinished);
    connect(m_process, &QProcess::started, this, [this]() {
        m_processStatusLabel->setText(QString("状态: 运行中 (PID: %1)")
            .arg(m_process->processId()));
        m_processStatusLabel->setStyleSheet("font-weight: bold; color: green; padding: 5px;");
    });

    // 预设命令
    connect(presetLs, &QPushButton::clicked, this, [this]() { m_processCmdEdit->setText("ls -la"); });
    connect(presetDate, &QPushButton::clicked, this, [this]() { m_processCmdEdit->setText("date"); });
    connect(presetUname, &QPushButton::clicked, this, [this]() { m_processCmdEdit->setText("uname -a"); });
    connect(presetPwd, &QPushButton::clicked, this, [this]() { m_processCmdEdit->setText("pwd"); });
}

void MainWindow::onProcessStart()
{
    QString cmd = m_processCmdEdit->text().trimmed();
    if (cmd.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入要执行的命令");
        return;
    }

    m_processOutput->clear();
    m_processLog->clear();

    // 解析命令和参数
    QStringList args = cmd.split(' ', QString::SkipEmptyParts);
    QString program = args.takeFirst();

    m_processLog->append(QString("[%1] 启动进程: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(program));
    if (!args.isEmpty()) {
        m_processLog->append(QString("  参数: %1").arg(args.join(" ")));
    }

    m_process->start(program, args);
    m_processStartBtn->setEnabled(false);
    m_processStopBtn->setEnabled(true);
    m_processWriteBtn->setEnabled(true);
}

void MainWindow::onProcessStop()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_processLog->append(QString("[%1] 进程已被终止")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
    }
}

void MainWindow::onProcessWrite()
{
    QString input = m_processInputEdit->text();
    if (input.isEmpty()) return;

    m_process->write((input + "\n").toUtf8());
    m_processLog->append(QString("[%1] 写入 stdin: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(input));
    m_processInputEdit->clear();
}

void MainWindow::readProcessOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    m_processOutput->append(QString::fromUtf8(data));
}

void MainWindow::readProcessError()
{
    QByteArray data = m_process->readAllStandardError();
    m_processOutput->append(QString("<span style=\"color:red;\">%1</span>")
        .arg(QString::fromUtf8(data).toHtmlEscaped()));
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    QString statusStr = (status == QProcess::NormalExit) ? "正常退出" : "异常退出";
    m_processStatusLabel->setText(QString("状态: %1 (退出码: %2)").arg(statusStr).arg(exitCode));
    m_processStatusLabel->setStyleSheet("font-weight: bold; color: blue; padding: 5px;");
    m_processLog->append(QString("[%1] 进程 %2, 退出码: %3")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(statusStr).arg(exitCode));
    m_processStartBtn->setEnabled(true);
    m_processStopBtn->setEnabled(false);
    m_processWriteBtn->setEnabled(false);
}

// ============================================================
// Tab 4: 综合示例 - 进程间消息传递
// ============================================================
void MainWindow::createIpcExampleTab(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(parent);

    // ---- 说明 ----
    QGroupBox *infoGroup = new QGroupBox("说明", parent);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    QLabel *infoLabel = new QLabel(
        "本示例演示本地套接字实现进程间消息传递:\n\n"
        "1. 点击 \"启动服务端\" 创建本地套接字服务\n"
        "2. 点击 \"连接客户端\" 自身连接到服务端\n"
        "   （实际场景中可启动另一个程序实例连接）\n"
        "3. 在消息框中输入内容，点击 \"发送消息\"\n"
        "4. 服务端收到后自动回复，展示双向通信", infoGroup);
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);
    mainLayout->addWidget(infoGroup);

    // ---- 状态 ----
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_ipcStatusLabel = new QLabel("状态: 未启动", parent);
    m_ipcStatusLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px; "
        "background-color: #ecf0f1; border-radius: 3px;");
    statusLayout->addWidget(m_ipcStatusLabel);
    statusLayout->addStretch();
    mainLayout->addLayout(statusLayout);

    // ---- 控制面板 ----
    QGroupBox *ctrlGroup = new QGroupBox("控制", parent);
    QHBoxLayout *ctrlLayout = new QHBoxLayout(ctrlGroup);

    ctrlLayout->addWidget(new QLabel("服务名称:", ctrlGroup));
    m_ipcServerNameEdit = new QLineEdit("qt_ipc_demo", ctrlGroup);
    ctrlLayout->addWidget(m_ipcServerNameEdit);

    m_ipcServerStartBtn = new QPushButton("启动服务端", ctrlGroup);
    m_ipcClientConnectBtn = new QPushButton("连接客户端", ctrlGroup);
    m_ipcClientConnectBtn->setEnabled(false);
    ctrlLayout->addWidget(m_ipcServerStartBtn);
    ctrlLayout->addWidget(m_ipcClientConnectBtn);
    ctrlLayout->addStretch();

    mainLayout->addWidget(ctrlGroup);

    // ---- 消息发送 ----
    QGroupBox *msgGroup = new QGroupBox("消息传递", parent);
    QHBoxLayout *msgLayout = new QHBoxLayout(msgGroup);

    msgLayout->addWidget(new QLabel("消息:", msgGroup));
    m_ipcClientMsgEdit = new QLineEdit(msgGroup);
    m_ipcClientMsgEdit->setPlaceholderText("输入要发送的消息");
    msgLayout->addWidget(m_ipcClientMsgEdit);

    m_ipcSendBtn = new QPushButton("发送消息", msgGroup);
    m_ipcSendBtn->setEnabled(false);
    msgLayout->addWidget(m_ipcSendBtn);

    mainLayout->addWidget(msgGroup);

    // ---- 通信日志 ----
    QGroupBox *logGroup = new QGroupBox("通信日志", parent);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    m_ipcLog = new QTextEdit(logGroup);
    m_ipcLog->setReadOnly(true);
    logLayout->addWidget(m_ipcLog);
    mainLayout->addWidget(logGroup, 1);

    // ---- 创建IPC服务端和客户端 ----
    m_ipcServer = new QLocalServer(this);
    m_ipcClient = new QLocalSocket(this);

    // ---- 信号连接 ----
    connect(m_ipcServerStartBtn, &QPushButton::clicked, this, &MainWindow::onIpcServerStart);
    connect(m_ipcClientConnectBtn, &QPushButton::clicked, this, &MainWindow::onIpcClientConnect);
    connect(m_ipcSendBtn, &QPushButton::clicked, this, &MainWindow::onIpcSendMessage);

    // 服务端新连接
    connect(m_ipcServer, &QLocalServer::newConnection, this, [this]() {
        QLocalSocket *incoming = m_ipcServer->nextPendingConnection();
        m_ipcLog->append(QString("[%1] [服务端] 新客户端已连接!")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
        m_ipcStatusLabel->setText("状态: 客户端已连接");
        m_ipcStatusLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px; "
            "background-color: #d5f5e3; border-radius: 3px;");

        // 读取客户端数据
        connect(incoming, &QLocalSocket::readyRead, this, [this, incoming]() {
            QByteArray data = incoming->readAll();
            QString msg = QString::fromUtf8(data);
            m_ipcLog->append(QString("[%1] [服务端] 收到: %2")
                .arg(QTime::currentTime().toString("HH:mm:ss")).arg(msg));

            // 自动回复
            QString reply = QString("服务端已收到: [%1] (长度:%2)")
                .arg(msg).arg(msg.length());
            incoming->write(reply.toUtf8());
            incoming->flush();
            m_ipcLog->append(QString("[%1] [服务端] 回复: %2")
                .arg(QTime::currentTime().toString("HH:mm:ss")).arg(reply));
        });

        connect(incoming, &QLocalSocket::disconnected, this, [this, incoming]() {
            m_ipcLog->append(QString("[%1] [服务端] 客户端已断开")
                .arg(QTime::currentTime().toString("HH:mm:ss")));
            incoming->deleteLater();
            m_ipcStatusLabel->setText("状态: 客户端已断开");
            m_ipcStatusLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px; "
                "background-color: #fadbd8; border-radius: 3px;");
        });
    });

    // 客户端读取服务端回复
    connect(m_ipcClient, &QLocalSocket::readyRead, this, [this]() {
        QByteArray data = m_ipcClient->readAll();
        m_ipcLog->append(QString("[%1] [客户端] 收到回复: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(QString::fromUtf8(data)));
    });

    connect(m_ipcClient, &QLocalSocket::disconnected, this, [this]() {
        m_ipcLog->append(QString("[%1] [客户端] 与服务端断开连接")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
        m_ipcSendBtn->setEnabled(false);
    });
}

void MainWindow::onIpcServerStart()
{
    QString serverName = m_ipcServerNameEdit->text().trimmed();
    if (serverName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入服务名称");
        return;
    }

    QLocalServer::removeServer(serverName);

    if (m_ipcServer->listen(serverName)) {
        m_ipcLog->append(QString("[%1] IPC 服务端启动成功, 监听: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss")).arg(serverName));
        m_ipcStatusLabel->setText("状态: 服务端运行中, 等待客户端连接...");
        m_ipcStatusLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px; "
            "background-color: #d4efdf; border-radius: 3px;");
        m_ipcServerStartBtn->setEnabled(false);
        m_ipcClientConnectBtn->setEnabled(true);
    } else {
        m_ipcLog->append(QString("[%1] 服务端启动失败: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(m_ipcServer->errorString()));
    }
}

void MainWindow::onIpcClientConnect()
{
    QString serverName = m_ipcServerNameEdit->text().trimmed();
    m_ipcLog->append(QString("[%1] [客户端] 正在连接到 %2...")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(serverName));

    m_ipcClient->connectToServer(serverName);

    // 等待连接结果
    if (m_ipcClient->waitForConnected(2000)) {
        m_ipcLog->append(QString("[%1] [客户端] 连接成功!")
            .arg(QTime::currentTime().toString("HH:mm:ss")));
        m_ipcSendBtn->setEnabled(true);
        m_ipcClientConnectBtn->setEnabled(false);
    } else {
        m_ipcLog->append(QString("[%1] [客户端] 连接失败: %2")
            .arg(QTime::currentTime().toString("HH:mm:ss"))
            .arg(m_ipcClient->errorString()));
    }
}

void MainWindow::onIpcSendMessage()
{
    QString msg = m_ipcClientMsgEdit->text().trimmed();
    if (msg.isEmpty()) return;

    m_ipcClient->write(msg.toUtf8());
    m_ipcClient->flush();

    m_ipcLog->append(QString("[%1] [客户端] 发送: %2")
        .arg(QTime::currentTime().toString("HH:mm:ss")).arg(msg));

    m_ipcClientMsgEdit->clear();
}
