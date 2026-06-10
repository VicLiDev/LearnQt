#ifndef TAB_MESSAGE_MONITOR_H
#define TAB_MESSAGE_MONITOR_H

#include <QWidget>
#include <QDateTime>

class MqttClientWrapper;
class QLabel;
class QTableWidget;
class QTextEdit;
class QTimer;

/**
 * @brief Tab 9: 消息监控仪表盘
 *
 * 实时监控 MQTT 消息收发状态，提供：
 *   1. 顶部统计面板：已发布数、已接收数、发布速率、接收速率、连接时长
 *   2. 中部最近消息表格：滚动显示最新消息（最多 200 条，自动裁剪）
 *   3. 底部主题订阅树：根据收到的消息自动构建主题层级结构
 *
 * 统计数据通过 1 秒定时器刷新，速率计算采用简单增量法。
 */
class TabMessageMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TabMessageMonitor(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                           quint8 qos, bool retain, const QDateTime &timestamp); // 收到消息
    void onMessagePublished(quint32 msgId, const QString &topic,
                            const QByteArray &payload, quint8 qos);              // 发布消息
    void onPublishFailed(quint32 msgId, const QString &topic,
                          const QString &error);                                  // 发布失败
    void onConnectionStateChanged(bool connected);                                // 连接状态变化
    void onStatsRefresh();    // 每秒刷新统计数据

private:
    void setupUI();
    QWidget *createStatCard(const QString &title, const QString &value,
                            const QString &colorClass);
    void updateStatCard(QWidget *card, const QString &value);
    void addMessageToTable(const QString &direction, const QString &topic,
                           const QString &payload, quint8 qos);
    void addTopicToTree(const QString &topic);
    void trimMessageTable();

    MqttClientWrapper *m_wrapper;

    // 顶部统计标签
    QLabel        *m_publishRateLabel;      // 发布速率（条/秒）
    QLabel        *m_subscribeRateLabel;    // 接收速率（条/秒）
    QLabel        *m_totalPublishLabel;     // 总发布数
    QLabel        *m_totalSubscribeLabel;    // 总接收数
    QLabel        *m_connectionDurationLabel; // 连接时长

    // 统计卡片（用于设置样式）
    QWidget       *m_publishCard;
    QWidget       *m_subscribeCard;
    QWidget       *m_publishRateCard;
    QWidget       *m_subscribeRateCard;
    QWidget       *m_durationCard;

    // 中部最近消息表格（列：时间, 方向, 主题, 内容, QoS）
    QTableWidget  *m_recentMessages;

    // 底部主题树
    QTextEdit     *m_topicTree;

    // 统计刷新定时器（1秒）
    QTimer        *m_statsTimer;

    // 计数器
    qint64 m_totalPublished;       // 总发布消息数
    qint64 m_totalReceived;        // 总接收消息数
    qint64 m_lastPublished;        // 上次刷新时的发布计数（用于计算速率）
    qint64 m_lastReceived;         // 上次刷新时的接收计数（用于计算速率）
    double m_publishRate;          // 当前发布速率
    double m_subscribeRate;        // 当前接收速率

    // 连接时长追踪
    bool m_connected;
    QDateTime m_connectedSince;

    // 消息表格最大行数
    static const int kMaxMessageRows = 200;
};

#endif // TAB_MESSAGE_MONITOR_H
