#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QMessageBox>
#include <QDateTime>

#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkInterface>
#include <QNetworkConfigurationManager>
#include <QNetworkAddressEntry>
#include <QSslConfiguration>

// ============================================================================
// Tab1: TCP 客户端
// ============================================================================

TcpClientTab::TcpClientTab(QWidget *parent)
    : QWidget(parent)
    , m_socket(new QTcpSocket(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 连接设置
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel("服务器地址："), 0, 0);
    m_hostInput = new QLineEdit("127.0.0.1");
    grid->addWidget(m_hostInput, 0, 1);
    grid->addWidget(new QLabel("端口："), 0, 2);
    m_portInput = new QLineEdit("12345");
    grid->addWidget(m_portInput, 0, 3);

    QPushButton *btnConnect = new QPushButton("连接");
    QPushButton *btnDisconnect = new QPushButton("断开");
    grid->addWidget(btnConnect, 0, 4);
    grid->addWidget(btnDisconnect, 0, 5);
    layout->addLayout(grid);

    // 发送消息
    QHBoxLayout *sendLayout = new QHBoxLayout();
    sendLayout->addWidget(new QLabel("消息："));
    m_msgInput = new QLineEdit();
    m_msgInput->setPlaceholderText("输入要发送的消息...");
    sendLayout->addWidget(m_msgInput);
    QPushButton *btnSend = new QPushButton("发送");
    sendLayout->addWidget(btnSend);
    layout->addLayout(sendLayout);

    // 显示区域
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(new QLabel("通信记录："));
    layout->addWidget(m_display);

    // 信号连接
    connect(btnConnect, &QPushButton::clicked, this, &TcpClientTab::onConnect);
    connect(btnDisconnect, &QPushButton::clicked, this, &TcpClientTab::onDisconnect);
    connect(btnSend, &QPushButton::clicked, this, &TcpClientTab::onSend);

    connect(m_socket, &QTcpSocket::connected, this, &TcpClientTab::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClientTab::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClientTab::onReadyRead);
    connect(m_socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &TcpClientTab::onSocketError);

    m_display->append("[系统] TCP 客户端就绪，请输入服务器地址和端口后点击连接。");
}

void TcpClientTab::onConnect()
{
    QString host = m_hostInput->text().trimmed();
    quint16 port = m_portInput->text().toUShort();

    if (host.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入服务器地址！");
        return;
    }

    m_display->append(QString("[系统] 正在连接 %1:%2 ...").arg(host).arg(port));
    m_socket->connectToHost(host, port);
}

void TcpClientTab::onDisconnect()
{
    m_socket->disconnectFromHost();
    m_display->append("[系统] 已断开连接。");
}

void TcpClientTab::onSend()
{
    if (m_socket->state() != QAbstractSocket::ConnectedState) {
        m_display->append("[警告] 未连接到服务器！");
        return;
    }

    QString msg = m_msgInput->text().trimmed();
    if (msg.isEmpty()) return;

    QByteArray data = msg.toUtf8();
    m_socket->write(data);
    m_socket->flush();
    m_display->append(QString("[发送] %1").arg(msg));
    m_msgInput->clear();
}

void TcpClientTab::onConnected()
{
    m_display->append(QString("[系统] 已连接到 %1:%2")
        .arg(m_socket->peerAddress().toString()).arg(m_socket->peerPort()));
}

void TcpClientTab::onDisconnected()
{
    m_display->append("[系统] 连接已断开。");
}

void TcpClientTab::onReadyRead()
{
    while (m_socket->canReadLine()) {
        QString line = QString::fromUtf8(m_socket->readLine()).trimmed();
        m_display->append(QString("[接收] %1").arg(line));
    }

    // 如果没有换行符的数据
    if (m_socket->bytesAvailable() > 0) {
        QByteArray data = m_socket->readAll();
        m_display->append(QString("[接收] %1").arg(QString::fromUtf8(data)));
    }
}

void TcpClientTab::onSocketError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err)
    m_display->append(QString("[错误] %1").arg(m_socket->errorString()));
}

// ============================================================================
// Tab2: TCP 服务端
// ============================================================================

TcpServerTab::TcpServerTab(QWidget *parent)
    : QWidget(parent)
    , m_server(new QTcpServer(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 服务端控制
    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(new QLabel("监听端口："));
    m_portInput = new QLineEdit("12345");
    ctrlLayout->addWidget(m_portInput);
    QPushButton *btnStart = new QPushButton("启动服务");
    QPushButton *btnStop = new QPushButton("停止服务");
    ctrlLayout->addWidget(btnStart);
    ctrlLayout->addWidget(btnStop);
    layout->addLayout(ctrlLayout);

    // 客户端列表和通信记录
    QHBoxLayout *midLayout = new QHBoxLayout();

    QVBoxLayout *clientLayout = new QVBoxLayout();
    clientLayout->addWidget(new QLabel("已连接客户端："));
    m_clientList = new QListWidget();
    m_clientList->setMinimumWidth(200);
    clientLayout->addWidget(m_clientList);
    midLayout->addLayout(clientLayout, 1);

    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(new QLabel("通信记录："));
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    logLayout->addWidget(m_display);
    midLayout->addLayout(logLayout, 2);

    layout->addLayout(midLayout);

    // 发送消息
    QHBoxLayout *sendLayout = new QHBoxLayout();
    sendLayout->addWidget(new QLabel("消息："));
    m_msgInput = new QLineEdit();
    m_msgInput->setPlaceholderText("输入广播消息...");
    sendLayout->addWidget(m_msgInput);
    QPushButton *btnBroadcast = new QPushButton("广播给所有");
    QPushButton *btnSendToClient = new QPushButton("发送给选中");
    sendLayout->addWidget(btnBroadcast);
    sendLayout->addWidget(btnSendToClient);
    layout->addLayout(sendLayout);

    // 信号连接
    connect(btnStart, &QPushButton::clicked, this, &TcpServerTab::onStartServer);
    connect(btnStop, &QPushButton::clicked, this, &TcpServerTab::onStopServer);
    connect(btnBroadcast, &QPushButton::clicked, this, &TcpServerTab::onBroadcast);
    connect(btnSendToClient, &QPushButton::clicked, this, &TcpServerTab::onSendToClient);

    connect(m_server, &QTcpServer::newConnection, this, &TcpServerTab::onNewConnection);

    m_display->append("[系统] TCP 服务端就绪，请设置端口后启动服务。");
}

void TcpServerTab::onStartServer()
{
    quint16 port = m_portInput->text().toUShort();
    if (port == 0) {
        QMessageBox::warning(this, "警告", "请输入有效端口号！");
        return;
    }

    if (!m_server->listen(QHostAddress::Any, port)) {
        m_display->append(QString("[错误] 无法启动服务：%1").arg(m_server->errorString()));
        return;
    }

    m_display->append(QString("[系统] 服务已启动，正在监听端口 %1").arg(port));
    m_display->append(QString("[系统] 本机地址列表："));
    foreach (const QHostAddress &addr, QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol &&
            addr != QHostAddress(QHostAddress::LocalHost)) {
            m_display->append(QString("  %1:%2").arg(addr.toString()).arg(port));
        }
    }
    m_display->append(QString("  127.0.0.1:%1").arg(port));
}

void TcpServerTab::onStopServer()
{
    // 断开所有客户端
    foreach (QTcpSocket *client, m_clients) {
        client->disconnectFromHost();
    }
    m_clients.clear();
    m_clientList->clear();

    m_server->close();
    m_display->append("[系统] 服务已停止。");
}

void TcpServerTab::onBroadcast()
{
    QString msg = m_msgInput->text().trimmed();
    if (msg.isEmpty()) return;

    if (m_clients.isEmpty()) {
        m_display->append("[警告] 没有已连接的客户端！");
        return;
    }

    QByteArray data = (msg + "\n").toUtf8();
    foreach (QTcpSocket *client, m_clients) {
        client->write(data);
        client->flush();
    }
    m_display->append(QString("[广播] %1 （发送给 %2 个客户端）").arg(msg).arg(m_clients.size()));
    m_msgInput->clear();
}

void TcpServerTab::onSendToClient()
{
    QString msg = m_msgInput->text().trimmed();
    if (msg.isEmpty()) return;

    int row = m_clientList->currentRow();
    if (row < 0 || row >= m_clients.size()) {
        m_display->append("[警告] 请先选择一个客户端！");
        return;
    }

    QTcpSocket *client = m_clients[row];
    QByteArray data = (msg + "\n").toUtf8();
    client->write(data);
    client->flush();
    m_display->append(QString("[发送] -> 客户端%1：%2").arg(row + 1).arg(msg));
    m_msgInput->clear();
}

void TcpServerTab::onNewConnection()
{
    QTcpSocket *client = m_server->nextPendingConnection();
    m_clients.append(client);

    QString clientInfo = QString("%1:%2")
        .arg(client->peerAddress().toString()).arg(client->peerPort());
    m_clientList->addItem(QString("客户端 %1 - %2").arg(m_clients.size()).arg(clientInfo));

    m_display->append(QString("[系统] 新客户端连接：%1").arg(clientInfo));

    connect(client, &QTcpSocket::readyRead, this, &TcpServerTab::onClientReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &TcpServerTab::onClientDisconnected);
}

void TcpServerTab::onClientReadyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if (!client) return;

    while (client->canReadLine()) {
        QString line = QString::fromUtf8(client->readLine()).trimmed();
        QString clientAddr = QString("%1:%2")
            .arg(client->peerAddress().toString()).arg(client->peerPort());
        m_display->append(QString("[接收] 来自 %1：%2").arg(clientAddr, line));

        // 回显给该客户端
        client->write(QString("[服务端回显] %1\n").arg(line).toUtf8());
        client->flush();
    }
}

void TcpServerTab::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if (!client) return;

    int index = m_clients.indexOf(client);
    if (index >= 0) {
        m_clientList->takeItem(index);
        m_clients.removeAt(index);
        m_display->append(QString("[系统] 客户端已断开：%1:%2")
            .arg(client->peerAddress().toString()).arg(client->peerPort()));
    }

    client->deleteLater();
}

// ============================================================================
// Tab3: UDP 通信
// ============================================================================

UdpTab::UdpTab(QWidget *parent)
    : QWidget(parent)
    , m_udpSocket(new QUdpSocket(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 绑定本地端口
    QHBoxLayout *bindLayout = new QHBoxLayout();
    bindLayout->addWidget(new QLabel("本地绑定端口："));
    m_localPortInput = new QLineEdit("12346");
    bindLayout->addWidget(m_localPortInput);
    QPushButton *btnBind = new QPushButton("绑定端口");
    bindLayout->addWidget(btnBind);
    layout->addLayout(bindLayout);

    // 发送设置
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel("目标地址："), 0, 0);
    m_targetHostInput = new QLineEdit("127.0.0.1");
    grid->addWidget(m_targetHostInput, 0, 1);
    grid->addWidget(new QLabel("目标端口："), 0, 2);
    m_targetPortInput = new QLineEdit("12346");
    grid->addWidget(m_targetPortInput, 0, 3);
    layout->addLayout(grid);

    // 消息输入
    QHBoxLayout *sendLayout = new QHBoxLayout();
    sendLayout->addWidget(new QLabel("消息："));
    m_msgInput = new QLineEdit();
    m_msgInput->setPlaceholderText("输入 UDP 消息...");
    sendLayout->addWidget(m_msgInput);
    QPushButton *btnUnicast = new QPushButton("单播发送");
    QPushButton *btnBroadcast = new QPushButton("广播发送");
    sendLayout->addWidget(btnUnicast);
    sendLayout->addWidget(btnBroadcast);
    layout->addLayout(sendLayout);

    // 显示区域
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(new QLabel("UDP 通信记录："));
    layout->addWidget(m_display);

    // 信号连接
    connect(btnBind, &QPushButton::clicked, this, &UdpTab::onBindPort);
    connect(btnUnicast, &QPushButton::clicked, this, &UdpTab::onSendUnicast);
    connect(btnBroadcast, &QPushButton::clicked, this, &UdpTab::onSendBroadcast);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpTab::onReadyRead);

    m_display->append("[系统] UDP 就绪。请先绑定本地端口，然后可以发送消息。");
    m_display->append("[提示] 单播/广播发送到目标地址:端口。");
    m_display->append("[提示] 绑定 12346 后发送到 127.0.0.1:12346 可自测。");
}

void UdpTab::onBindPort()
{
    quint16 port = m_localPortInput->text().toUShort();
    if (m_udpSocket->state() == QAbstractSocket::BoundState) {
        m_udpSocket->close();
        m_display->append("[系统] 已关闭之前的绑定。");
    }

    if (m_udpSocket->bind(QHostAddress::Any, port)) {
        m_display->append(QString("[系统] 已绑定端口 %1，等待接收数据...").arg(port));
    } else {
        m_display->append(QString("[错误] 绑定端口失败：%1").arg(m_udpSocket->errorString()));
    }
}

void UdpTab::onSendUnicast()
{
    QString host = m_targetHostInput->text().trimmed();
    quint16 port = m_targetPortInput->text().toUShort();
    QString msg = m_msgInput->text().trimmed();
    if (msg.isEmpty()) return;

    QByteArray data = msg.toUtf8();
    qint64 sent = m_udpSocket->writeDatagram(data, QHostAddress(host), port);
    m_display->append(QString("[单播] -> %1:%2 （%3 字节）：%4")
        .arg(host).arg(port).arg(sent).arg(msg));
    m_msgInput->clear();
}

void UdpTab::onSendBroadcast()
{
    quint16 port = m_targetPortInput->text().toUShort();
    QString msg = m_msgInput->text().trimmed();
    if (msg.isEmpty()) return;

    QByteArray data = msg.toUtf8();
    qint64 sent = m_udpSocket->writeDatagram(data, QHostAddress::Broadcast, port);
    m_display->append(QString("[广播] -> 255.255.255.255:%1 （%2 字节）：%3")
        .arg(port).arg(sent).arg(msg));
    m_msgInput->clear();
}

void UdpTab::onReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress senderAddr;
        quint16 senderPort;

        m_udpSocket->readDatagram(buffer.data(), buffer.size(), &senderAddr, &senderPort);

        QString msg = QString::fromUtf8(buffer);
        m_display->append(QString("[接收] 来自 %1:%2：%3")
            .arg(senderAddr.toString()).arg(senderPort).arg(msg));
    }
}

// ============================================================================
// Tab4: HTTP 客户端
// ============================================================================

HttpClientTab::HttpClientTab(QWidget *parent)
    : QWidget(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // URL 输入
    QHBoxLayout *urlLayout = new QHBoxLayout();
    urlLayout->addWidget(new QLabel("URL："));
    m_urlInput = new QLineEdit("https://httpbin.org/get");
    urlLayout->addWidget(m_urlInput);
    QPushButton *btnGet = new QPushButton("GET 请求");
    urlLayout->addWidget(btnGet);
    layout->addLayout(urlLayout);

    // POST 数据
    QHBoxLayout *postLayout = new QHBoxLayout();
    postLayout->addWidget(new QLabel("POST 数据："));
    m_postDataInput = new QLineEdit("{\"key\":\"value\",\"msg\":\"你好\"}");
    postLayout->addWidget(m_postDataInput);
    QPushButton *btnPost = new QPushButton("POST 请求");
    postLayout->addWidget(btnPost);
    layout->addLayout(postLayout);

    // 显示区域
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(new QLabel("HTTP 响应："));
    layout->addWidget(m_display);

    // 信号连接
    connect(btnGet, &QPushButton::clicked, this, &HttpClientTab::onGetRequest);
    connect(btnPost, &QPushButton::clicked, this, &HttpClientTab::onPostRequest);

    m_display->append("[系统] HTTP 客户端就绪。\n");
    m_display->append("[提示] GET 请求默认访问 httpbin.org，需要网络连接。\n");
    m_display->append("[提示] 可修改 URL 为任意 HTTP 地址进行测试。");
}

void HttpClientTab::onGetRequest()
{
    QString urlStr = m_urlInput->text().trimmed();
    if (urlStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入 URL！");
        return;
    }

    QUrl url(urlStr);
    if (!url.isValid()) {
        m_display->append(QString("[错误] 无效的 URL：%1").arg(url.errorString()));
        return;
    }

    QNetworkRequest request(url);
    // 设置自定义请求头
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt5 Network Demo/1.0");
    request.setRawHeader("Accept", "application/json, text/plain, */*");

    m_display->append(QString("\n[GET] %1").arg(urlStr));
    m_display->append("[系统] 请求已发送，等待响应...");

    QNetworkReply *reply = m_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void HttpClientTab::onPostRequest()
{
    QString urlStr = m_urlInput->text().trimmed();
    if (urlStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入 URL！");
        return;
    }

    QUrl url(urlStr);
    if (!url.isValid()) {
        m_display->append(QString("[错误] 无效的 URL：%1").arg(url.errorString()));
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt5 Network Demo/1.0");

    QByteArray postData = m_postDataInput->text().toUtf8();

    m_display->append(QString("\n[POST] %1").arg(urlStr));
    m_display->append(QString("[POST] 数据：%1").arg(QString::fromUtf8(postData)));
    m_display->append("[系统] 请求已发送，等待响应...");

    QNetworkReply *reply = m_manager->post(request, postData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void HttpClientTab::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // 获取响应状态码
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        m_display->append(QString("[响应] 状态码：%1").arg(statusCode.toInt()));

        // 获取响应头
        m_display->append("[响应] 响应头：");
        foreach (const QByteArray &headerName, reply->rawHeaderList()) {
            m_display->append(QString("  %1: %2")
                .arg(QString::fromUtf8(headerName))
                .arg(QString::fromUtf8(reply->rawHeader(headerName))));
        }

        // 获取响应体
        QByteArray body = reply->readAll();
        m_display->append(QString("\n[响应体]（%1 字节）：\n%2")
            .arg(body.size()).arg(QString::fromUtf8(body)));
    } else {
        m_display->append(QString("[错误] %1").arg(reply->errorString()));
    }

    reply->deleteLater();
}

// ============================================================================
// Tab5: 网络信息
// ============================================================================

NetworkInfoTab::NetworkInfoTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *btnRefresh = new QPushButton("刷新网络接口信息");
    QPushButton *btnCheckOnline = new QPushButton("检测在线状态");
    layout->addWidget(btnRefresh);
    layout->addWidget(btnCheckOnline);

    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(m_display);

    connect(btnRefresh, &QPushButton::clicked, this, &NetworkInfoTab::onRefreshInterfaces);
    connect(btnCheckOnline, &QPushButton::clicked, this, &NetworkInfoTab::onCheckOnline);

    // 自动检测在线状态变化
    QNetworkConfigurationManager *ncm = new QNetworkConfigurationManager(this);
    connect(ncm, &QNetworkConfigurationManager::onlineStateChanged,
            this, &NetworkInfoTab::onOnlineStateChanged);
}

void NetworkInfoTab::onRefreshInterfaces()
{
    QString result;
    result += "===== 网络接口信息 =====\n\n";

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    result += QString("共 %1 个网络接口\n\n").arg(interfaces.size());

    foreach (const QNetworkInterface &iface, interfaces) {
        result += QString("--- %1 (%2) ---\n").arg(iface.humanReadableName(), iface.name());
        result += QString("  硬件地址：%1\n").arg(iface.hardwareAddress());
        result += QString("  状态：%1\n").arg(iface.flags() & QNetworkInterface::IsUp ? "启用" : "禁用");
        result += QString("  是否回环：%1\n").arg(iface.flags() & QNetworkInterface::IsLoopBack ? "是" : "否");

        // 列出 IP 地址
        QList<QNetworkAddressEntry> entries = iface.addressEntries();
        foreach (const QNetworkAddressEntry &entry, entries) {
            QString protocol = entry.ip().protocol() == QAbstractSocket::IPv4Protocol ? "IPv4" : "IPv6";
            result += QString("  %1 地址：%2/%3 (广播：%4)\n")
                .arg(protocol)
                .arg(entry.ip().toString())
                .arg(entry.prefixLength())
                .arg(entry.broadcast().toString());
        }
        result += "\n";
    }

    m_display->setPlainText(result);
}

void NetworkInfoTab::onCheckOnline()
{
    QNetworkConfigurationManager ncm;

    QString result = "===== 在线状态检测 =====\n\n";
    result += QString("当前在线状态：%1\n").arg(ncm.isOnline() ? "在线" : "离线");

    // 网络配置信息
    result += "\n默认配置：\n";
    QNetworkConfiguration defaultConfig = ncm.defaultConfiguration();
    if (defaultConfig.isValid()) {
        result += QString("  名称：%1\n").arg(defaultConfig.name());
        result += QString("  类型：%1\n").arg(defaultConfig.bearerTypeName());
        result += QString("  状态：%1\n").arg(
            defaultConfig.state() == QNetworkConfiguration::Active ? "活跃" : "不活跃");
    } else {
        result += "  无默认配置\n";
    }

    // 所有配置
    result += QString("\n所有网络配置（%1 个）：\n").arg(ncm.allConfigurations().count());
    foreach (const QNetworkConfiguration &config, ncm.allConfigurations()) {
        if (config.state() == QNetworkConfiguration::Active) {
            result += QString("  [活跃] %1 (%2)\n").arg(config.name(), config.bearerTypeName());
        }
    }

    m_display->setPlainText(result);
}

void NetworkInfoTab::onOnlineStateChanged(bool online)
{
    m_display->append(QString("\n[通知] 网络状态变化：%1").arg(online ? "在线" : "离线"));
}

// ============================================================================
// MainWindow 主窗口
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Demo 19 - 网络编程");
    resize(900, 700);

    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    m_tabWidget->addTab(new TcpClientTab(), "TCP 客户端");
    m_tabWidget->addTab(new TcpServerTab(), "TCP 服务端");
    m_tabWidget->addTab(new UdpTab(), "UDP 通信");
    m_tabWidget->addTab(new HttpClientTab(), "HTTP 客户端");
    m_tabWidget->addTab(new NetworkInfoTab(), "网络信息");
}
