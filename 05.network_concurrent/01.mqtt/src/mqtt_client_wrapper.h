#ifndef MQTT_CLIENT_WRAPPER_H
#define MQTT_CLIENT_WRAPPER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QSslConfiguration>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>
#include <QtMqtt/QMqttMessage>
#include <QtMqtt/QMqttTopicFilter>

/**
 * @brief MQTT 客户端封装类
 *
 * 对 QMqttClient 进行二次封装，提供以下增强功能：
 *   1. 统一的连接/断开/发布/订阅接口
 *   2. 自动重连（指数退避策略）
 *   3. 消息日志信号，方便各 Tab 监听
 *   4. 连接状态管理
 *   5. LWT（遗嘱消息）配置
 *   6. TLS/SSL 支持
 */
class MqttClientWrapper : public QObject
{
    Q_OBJECT

public:
    explicit MqttClientWrapper(QObject *parent = nullptr);
    ~MqttClientWrapper();

    // ========== 连接配置 ==========

    void setHost(const QString &host);
    void setPort(quint16 port);
    void setClientId(const QString &id);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setKeepAlive(int seconds);
    void setCleanSession(bool clean);

    // ========== TLS/SSL 配置 ==========

    void setSslEnabled(bool enabled);
    void setCaCertificates(const QList<QSslCertificate> &certs);
    void setLocalCertificate(const QSslCertificate &cert);
    void setPrivateKey(const QSslKey &key);
    void setSslProtocol(QSsl::SslProtocol protocol);

    // ========== LWT 遗嘱消息配置 ==========

    void setWillTopic(const QString &topic);
    void setWillMessage(const QByteArray &message);
    void setWillQoS(quint8 qos);
    void setWillRetain(bool retain);

    // ========== 连接操作 ==========

    void connectToBroker();
    void disconnectFromBroker();

    // ========== 发布与订阅 ==========

    quint32 publish(const QString &topic, const QByteArray &payload,
                    quint8 qos = 0, bool retain = false);
    QMqttSubscription *subscribe(const QString &topicFilter, quint8 qos = 0);
    void unsubscribe(const QString &topicFilter);

    // ========== 状态查询 ==========

    bool isConnected() const;
    QMqttClient *client() const;

signals:
    // 连接状态变化
    void connectionStateChanged(bool connected);
    void brokerDisconnected();

    // 消息日志（供各 Tab 统一监听）
    void messageReceived(const QString &topic, const QByteArray &payload,
                         quint8 qos, bool retain, const QDateTime &timestamp);
    void messagePublished(quint32 msgId, const QString &topic,
                          const QByteArray &payload, quint8 qos);
    void publishFailed(quint32 msgId, const QString &topic, const QString &error);

    // 订阅事件
    void subscribed(const QString &topicFilter, quint8 qos);
    void unsubscribed(const QString &topicFilter);

    // 通用日志
    void logMessage(const QString &msg, const QString &level = "info");

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorChanged(QMqttClient::ClientError error);
    void onStateChanged(QMqttClient::ClientState state);

private:
    void setupAutoReconnect();
    void applyWillConfig();

    QMqttClient *m_client;

    // 自动重连
    QTimer *m_reconnectTimer;
    int m_reconnectInterval;  // 当前重连间隔（ms）
    static const int kMaxReconnectInterval = 60000;  // 最大 60 秒
    static const int kMinReconnectInterval = 2000;   // 最小 2 秒

    // LWT 配置
    QString m_willTopic;
    QByteArray m_willMessage;
    quint8 m_willQos;
    bool m_willRetain;

    // SSL 配置
    bool m_sslEnabled;
    QSslConfiguration m_sslConfig;
};

#endif // MQTT_CLIENT_WRAPPER_H
