#include "mqtt_client_wrapper.h"
#include <QTimer>

// ============================================================
// 构造 / 析构
// ============================================================

MqttClientWrapper::MqttClientWrapper(QObject *parent)
    : QObject(parent)
    , m_client(new QMqttClient(this))
    , m_reconnectTimer(nullptr)
    , m_reconnectInterval(kMinReconnectInterval)
    , m_willQos(0)
    , m_willRetain(false)
    , m_sslEnabled(false)
{
    // ---- 默认配置 ----
    m_client->setHostname("broker.emqx.io");
    m_client->setPort(1883);
    m_client->setClientId("qt_mqtt_demo_" + QString::number(QDateTime::currentMSecsSinceEpoch() % 100000));
    m_client->setKeepAlive(60);
    m_client->setCleanSession(true);

    // ---- 信号连接 ----
    connect(m_client, &QMqttClient::connected,       this, &MqttClientWrapper::onConnected);
    connect(m_client, &QMqttClient::disconnected,    this, &MqttClientWrapper::onDisconnected);
    connect(m_client, &QMqttClient::errorChanged,     this, &MqttClientWrapper::onErrorChanged);
    connect(m_client, &QMqttClient::stateChanged,     this, &MqttClientWrapper::onStateChanged);

    setupAutoReconnect();
}

MqttClientWrapper::~MqttClientWrapper()
{
    if (m_reconnectTimer) {
        m_reconnectTimer->stop();
    }
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

// ============================================================
// 连接配置
// ============================================================

void MqttClientWrapper::setHost(const QString &host)
{
    m_client->setHostname(host);
}

void MqttClientWrapper::setPort(quint16 port)
{
    m_client->setPort(port);
}

void MqttClientWrapper::setClientId(const QString &id)
{
    m_client->setClientId(id);
}

void MqttClientWrapper::setUsername(const QString &username)
{
    m_client->setUsername(username);
}

void MqttClientWrapper::setPassword(const QString &password)
{
    m_client->setPassword(password);
}

void MqttClientWrapper::setKeepAlive(int seconds)
{
    m_client->setKeepAlive(seconds);
}

void MqttClientWrapper::setCleanSession(bool clean)
{
    m_client->setCleanSession(clean);
}

// ============================================================
// TLS/SSL 配置
// ============================================================

void MqttClientWrapper::setSslEnabled(bool enabled)
{
    m_sslEnabled = enabled;
}

void MqttClientWrapper::setCaCertificates(const QList<QSslCertificate> &certs)
{
    m_sslConfig.setCaCertificates(certs);
}

void MqttClientWrapper::setLocalCertificate(const QSslCertificate &cert)
{
    m_sslConfig.setLocalCertificate(cert);
}

void MqttClientWrapper::setPrivateKey(const QSslKey &key)
{
    m_sslConfig.setPrivateKey(key);
}

void MqttClientWrapper::setSslProtocol(QSsl::SslProtocol protocol)
{
    m_sslConfig.setProtocol(protocol);
}

// ============================================================
// LWT 遗嘱消息配置
// ============================================================

void MqttClientWrapper::setWillTopic(const QString &topic)    { m_willTopic   = topic; }
void MqttClientWrapper::setWillMessage(const QByteArray &msg) { m_willMessage = msg; }
void MqttClientWrapper::setWillQoS(quint8 qos)                { m_willQos     = qos; }
void MqttClientWrapper::setWillRetain(bool retain)            { m_willRetain  = retain; }

// ============================================================
// 连接操作
// ============================================================

void MqttClientWrapper::connectToBroker()
{
    // 应用遗嘱消息
    applyWillConfig();

    logMessage(QString("正在连接 %1:%2 ...")
               .arg(m_client->hostname()).arg(m_client->port()));

    if (m_sslEnabled) {
        m_client->connectToHostEncrypted(m_sslConfig);
        logMessage("TLS/SSL 已启用，使用加密连接", "info");
    } else {
        m_client->connectToHost();
    }
}

void MqttClientWrapper::disconnectFromBroker()
{
    if (m_reconnectTimer) {
        m_reconnectTimer->stop();    // 断开时停止自动重连
    }
    m_reconnectInterval = kMinReconnectInterval;

    if (m_client->state() != QMqttClient::Disconnected) {
        logMessage("正在断开连接...");
        m_client->disconnectFromHost();
    }
}

// ============================================================
// 发布与订阅
// ============================================================

quint32 MqttClientWrapper::publish(const QString &topic, const QByteArray &payload,
                                  quint8 qos, bool retain)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit publishFailed(0, topic, "未连接到 Broker");
        logMessage(QString("发布失败 [%1]: 未连接").arg(topic), "error");
        return 0;
    }

    quint32 msgId = m_client->publish(topic, payload, qos, retain);
    emit messagePublished(msgId, topic, payload, qos);
    logMessage(QString("发布 [%1] QoS=%2 retain=%3 id=%4")
               .arg(topic).arg(qos).arg(retain).arg(msgId));
    return msgId;
}

QMqttSubscription *MqttClientWrapper::subscribe(const QString &topicFilter, quint8 qos)
{
    if (m_client->state() != QMqttClient::Connected) {
        logMessage(QString("订阅失败 [%1]: 未连接").arg(topicFilter), "error");
        return nullptr;
    }

    QMqttSubscription *sub = m_client->subscribe(QMqttTopicFilter(topicFilter), qos);
    if (!sub) {
        logMessage(QString("订阅失败 [%1]: Broker 拒绝").arg(topicFilter), "error");
        return nullptr;
    }

    // 订阅成功后，转发收到的消息到统一信号
    connect(sub, &QMqttSubscription::messageReceived,
            this, [this](const QMqttMessage &msg) {
                emit messageReceived(msg.topic().name(),
                                     msg.payload(),
                                     msg.qos(),
                                     msg.retain(),
                                     QDateTime::currentDateTime());
            });

    emit subscribed(topicFilter, qos);
    logMessage(QString("订阅成功 [%1] QoS=%2").arg(topicFilter).arg(qos));
    return sub;
}

void MqttClientWrapper::unsubscribe(const QString &topicFilter)
{
    if (m_client->state() != QMqttClient::Connected) {
        logMessage(QString("取消订阅失败 [%1]: 未连接").arg(topicFilter), "error");
        return;
    }
    // QMqttClient 没有直接的 unsubscribe 方法，需要通过 subscription 对象
    // 这里记录日志，实际取消订阅由各 Tab 管理 subscription 指针
    logMessage(QString("取消订阅 [%1]").arg(topicFilter));
    emit unsubscribed(topicFilter);
}

// ============================================================
// 状态查询
// ============================================================

bool MqttClientWrapper::isConnected() const
{
    return m_client->state() == QMqttClient::Connected;
}

QMqttClient *MqttClientWrapper::client() const
{
    return m_client;
}

// ============================================================
// 私有槽
// ============================================================

void MqttClientWrapper::onConnected()
{
    m_reconnectInterval = kMinReconnectInterval;  // 连上后重置重连间隔
    if (m_reconnectTimer) m_reconnectTimer->stop();
    emit connectionStateChanged(true);
    logMessage(QString("已连接到 %1:%2 (ClientID: %3)")
               .arg(m_client->hostname()).arg(m_client->port()).arg(m_client->clientId()));
}

void MqttClientWrapper::onDisconnected()
{
    emit connectionStateChanged(false);
    emit brokerDisconnected();
    logMessage("已断开连接");
}

void MqttClientWrapper::onErrorChanged(QMqttClient::ClientError error)
{
    if (error == QMqttClient::NoError) return;
    logMessage(QString("MQTT 错误: %1").arg(static_cast<int>(error)), "error");
}

void MqttClientWrapper::onStateChanged(QMqttClient::ClientState state)
{
    QString stateStr;
    switch (state) {
    case QMqttClient::Disconnected: stateStr = "未连接"; break;
    case QMqttClient::Connecting:    stateStr = "连接中"; break;
    case QMqttClient::Connected:     stateStr = "已连接"; break;
    default:                         stateStr = "未知"; break;
    }
    logMessage(QString("状态变更 → %1").arg(stateStr));
}

// ============================================================
// 自动重连（指数退避）
// ============================================================

void MqttClientWrapper::setupAutoReconnect()
{
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        logMessage(QString("自动重连中... (间隔 %1ms)").arg(m_reconnectInterval));
        m_client->connectToHost();
    });
}

// ============================================================
// 辅助方法
// ============================================================

void MqttClientWrapper::applyWillConfig()
{
    if (!m_willTopic.isEmpty()) {
        m_client->setWillTopic(m_willTopic);
        m_client->setWillMessage(m_willMessage);
        m_client->setWillQoS(m_willQos);
        m_client->setWillRetain(m_willRetain);
        logMessage(QString("LWT 已配置: topic=%1").arg(m_willTopic));
    }
}
