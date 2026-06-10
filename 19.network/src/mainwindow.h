#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QListWidget;
class QTcpSocket;
class QTcpServer;
class QUdpSocket;
class QNetworkAccessManager;
class QNetworkReply;

// 前向声明需要用完整类型，所以包含头文件
#include <QAbstractSocket>

/* ========== Tab1: TCP 客户端 ========== */
class TcpClientTab : public QWidget
{
    Q_OBJECT
public:
    explicit TcpClientTab(QWidget *parent = nullptr);

private slots:
    void onConnect();
    void onDisconnect();
    void onSend();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError err);

private:
    QLineEdit *m_hostInput;
    QLineEdit *m_portInput;
    QLineEdit *m_msgInput;
    QTextEdit *m_display;
    QTcpSocket *m_socket;
};

/* ========== Tab2: TCP 服务端 ========== */
class TcpServerTab : public QWidget
{
    Q_OBJECT
public:
    explicit TcpServerTab(QWidget *parent = nullptr);

private slots:
    void onStartServer();
    void onStopServer();
    void onBroadcast();
    void onSendToClient();
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();

private:
    QLineEdit *m_portInput;
    QLineEdit *m_msgInput;
    QTextEdit *m_display;
    QListWidget *m_clientList;
    QTcpServer *m_server;
    QList<QTcpSocket *> m_clients;
};

/* ========== Tab3: UDP 通信 ========== */
class UdpTab : public QWidget
{
    Q_OBJECT
public:
    explicit UdpTab(QWidget *parent = nullptr);

private slots:
    void onBindPort();
    void onSendUnicast();
    void onSendBroadcast();
    void onReadyRead();

private:
    QLineEdit *m_targetHostInput;
    QLineEdit *m_targetPortInput;
    QLineEdit *m_localPortInput;
    QLineEdit *m_msgInput;
    QTextEdit *m_display;
    QUdpSocket *m_udpSocket;
};

/* ========== Tab4: HTTP 客户端 ========== */
class HttpClientTab : public QWidget
{
    Q_OBJECT
public:
    explicit HttpClientTab(QWidget *parent = nullptr);

private slots:
    void onGetRequest();
    void onPostRequest();
    void onReplyFinished(QNetworkReply *reply);

private:
    QLineEdit *m_urlInput;
    QLineEdit *m_postDataInput;
    QTextEdit *m_display;
    QNetworkAccessManager *m_manager;
};

/* ========== Tab5: 网络信息 ========== */
class NetworkInfoTab : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkInfoTab(QWidget *parent = nullptr);

private slots:
    void onRefreshInterfaces();
    void onCheckOnline();
    void onOnlineStateChanged(bool online);

private:
    QTextEdit *m_display;
};

/* ========== 主窗口 ========== */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QTabWidget *m_tabWidget;
};

#endif // MAINWINDOW_H
