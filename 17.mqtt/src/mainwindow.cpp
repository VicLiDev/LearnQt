#include "mainwindow.h"
#include "tab_publish_subscribe.h"
#include "tab_qos_level.h"
#include "tab_topic_wildcard.h"
#include "tab_lwt.h"
#include "tab_retained.h"
#include "tab_auth_tls.h"
#include "tab_request_response.h"
#include "tab_batch_publish.h"
#include "tab_message_monitor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QTabWidget>
#include <QDateTime>
#include <QSplitter>

// ============================================================
// 构造 / 析构
// ============================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mqttClient(new MqttClientWrapper(this))
    , m_tabWidget(nullptr)
    , m_tabPubSub(nullptr)
    , m_tabQos(nullptr)
    , m_tabWildcard(nullptr)
    , m_tabLwt(nullptr)
    , m_tabRetained(nullptr)
    , m_tabAuthTls(nullptr)
    , m_tabReqResp(nullptr)
    , m_tabBatch(nullptr)
    , m_tabMonitor(nullptr)
{
    setupUI();

    // MQTT 客户端信号 → 主窗口
    connect(m_mqttClient, &MqttClientWrapper::connectionStateChanged,
            this, &MainWindow::onConnectionStateChanged);
    connect(m_mqttClient, &MqttClientWrapper::logMessage,
            this, &MainWindow::onLogMessage);
}

MainWindow::~MainWindow()
{
    m_mqttClient->disconnectFromBroker();
}

MqttClientWrapper *MainWindow::mqttClient() const
{
    return m_mqttClient;
}

// ============================================================
// UI 构建
// ============================================================

void MainWindow::setupUI()
{
    setWindowTitle("Qt MQTT 综合学习 Demo");
    resize(1200, 750);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // ---- 左侧连接面板 ----
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(280);
    setupConnectionPanel(leftPanel);
    mainLayout->addWidget(leftPanel);

    // ---- 右侧 TabWidget ----
    m_tabWidget = new QTabWidget();

    m_tabPubSub   = new TabPublishSubscribe(m_mqttClient);
    m_tabQos      = new TabQosLevel(m_mqttClient);
    m_tabWildcard = new TabTopicWildcard(m_mqttClient);
    m_tabLwt      = new TabLwt(m_mqttClient);
    m_tabRetained = new TabRetained(m_mqttClient);
    m_tabAuthTls  = new TabAuthTls(m_mqttClient);
    m_tabReqResp  = new TabRequestResponse(m_mqttClient);
    m_tabBatch    = new TabBatchPublish(m_mqttClient);
    m_tabMonitor  = new TabMessageMonitor(m_mqttClient);

    m_tabWidget->addTab(m_tabPubSub,   "① 基础发布/订阅");
    m_tabWidget->addTab(m_tabQos,      "② QoS 级别");
    m_tabWidget->addTab(m_tabWildcard, "③ 主题与通配符");
    m_tabWidget->addTab(m_tabLwt,      "④ Last Will 遗嘱");
    m_tabWidget->addTab(m_tabRetained, "⑤ 保留消息");
    m_tabWidget->addTab(m_tabAuthTls,  "⑥ 认证与TLS");
    m_tabWidget->addTab(m_tabReqResp,  "⑦ 请求-响应");
    m_tabWidget->addTab(m_tabBatch,    "⑧ 批量发布");
    m_tabWidget->addTab(m_tabMonitor,  "⑨ 消息监控");

    mainLayout->addWidget(m_tabWidget, 1);
}

void MainWindow::setupConnectionPanel(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);

    // ---- Broker 配置组 ----
    QGroupBox *groupConn = new QGroupBox("Broker 连接配置");
    QFormLayout *form = new QFormLayout(groupConn);

    m_editHost = new QLineEdit("broker.emqx.io");
    form->addRow("Host:", m_editHost);

    m_spinPort = new QSpinBox();
    m_spinPort->setRange(1, 65535);
    m_spinPort->setValue(1883);
    form->addRow("Port:", m_spinPort);

    m_editClientId = new QLineEdit(m_mqttClient->client()->clientId());
    form->addRow("Client ID:", m_editClientId);

    // ---- 认证组 ----
    QGroupBox *groupAuth = new QGroupBox("认证");
    QFormLayout *authForm = new QFormLayout(groupAuth);

    m_editUsername = new QLineEdit();
    m_editUsername->setPlaceholderText("(可选)");
    authForm->addRow("用户名:", m_editUsername);

    m_editPassword = new QLineEdit();
    m_editPassword->setPlaceholderText("(可选)");
    m_editPassword->setEchoMode(QLineEdit::Password);
    authForm->addRow("密码:", m_editPassword);

    // ---- 高级选项组 ----
    QGroupBox *groupAdv = new QGroupBox("高级选项");
    QFormLayout *advForm = new QFormLayout(groupAdv);

    m_spinKeepAlive = new QSpinBox();
    m_spinKeepAlive->setRange(10, 3600);
    m_spinKeepAlive->setValue(60);
    m_spinKeepAlive->setSuffix(" 秒");
    advForm->addRow("Keep Alive:", m_spinKeepAlive);

    m_checkCleanSession = new QCheckBox("Clean Session");
    m_checkCleanSession->setChecked(true);
    advForm->addRow(m_checkCleanSession);

    // ---- 连接按钮 ----
    m_btnConnect = new QPushButton("连接");
    m_btnConnect->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    connect(m_btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectClicked);

    m_btnDisconnect = new QPushButton("断开");
    m_btnDisconnect->setEnabled(false);
    m_btnDisconnect->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
    connect(m_btnDisconnect, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);

    // ---- 状态 ----
    m_labelStatus = new QLabel("● 未连接");
    m_labelStatus->setStyleSheet("QLabel { color: red; font-weight: bold; }");

    // ---- 日志区 ----
    QGroupBox *groupLog = new QGroupBox("日志");
    QVBoxLayout *logLayout = new QVBoxLayout(groupLog);
    m_textLog = new QTextEdit();
    m_textLog->setReadOnly(true);
    m_textLog->setFont(QFont("Consolas", 8));
    logLayout->addWidget(m_textLog);

    // 组装左侧面板
    layout->addWidget(groupConn);
    layout->addWidget(groupAuth);
    layout->addWidget(groupAdv);
    layout->addWidget(m_labelStatus);
    layout->addWidget(m_btnConnect);
    layout->addWidget(m_btnDisconnect);
    layout->addStretch();
    layout->addWidget(groupLog, 1);
}

// ============================================================
// 槽函数
// ============================================================

void MainWindow::onConnectClicked()
{
    // 将 UI 配置写入 wrapper
    m_mqttClient->setHost(m_editHost->text().trimmed());
    m_mqttClient->setPort(static_cast<quint16>(m_spinPort->value()));
    m_mqttClient->setClientId(m_editClientId->text().trimmed());
    m_mqttClient->setUsername(m_editUsername->text().trimmed());
    m_mqttClient->setPassword(m_editPassword->text().trimmed());
    m_mqttClient->setKeepAlive(m_spinKeepAlive->value());
    m_mqttClient->setCleanSession(m_checkCleanSession->isChecked());

    m_mqttClient->connectToBroker();
}

void MainWindow::onDisconnectClicked()
{
    m_mqttClient->disconnectFromBroker();
}

void MainWindow::onConnectionStateChanged(bool connected)
{
    if (connected) {
        m_labelStatus->setText("● 已连接");
        m_labelStatus->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        m_btnConnect->setEnabled(false);
        m_btnDisconnect->setEnabled(true);
    } else {
        m_labelStatus->setText("● 未连接");
        m_labelStatus->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        m_btnConnect->setEnabled(true);
        m_btnDisconnect->setEnabled(false);
    }
}

void MainWindow::onLogMessage(const QString &msg, const QString &level)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString color = (level == "error") ? "#ff4444"
                : (level == "warn")  ? "#ff8800"
                : "#333333";
    m_textLog->append(QString("<span style='color:gray'>[%1]</span> "
                              "<span style='color:%3'>%2</span>")
                       .arg(timestamp, msg, color));
}
