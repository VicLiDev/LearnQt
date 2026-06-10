#ifndef TAB_REQUEST_RESPONSE_H
#define TAB_REQUEST_RESPONSE_H

#include <QWidget>
#include <QMap>
#include <QDateTime>
#include <QString>

class MqttClientWrapper;
class QLineEdit;
class QTextEdit;
class QSpinBox;
class QPushButton;
class QTableWidget;
class QTimer;

/**
 * @brief Tab 7: 请求-响应模式
 *
 * 模拟 MQTT 上的 RPC（远程过程调用）请求-响应模式。
 * MQTT 本身是发布/订阅模型，没有原生的请求-响应语义。
 * 本 Tab 通过以下方式模拟该模式：
 *   1. 发送请求时生成唯一的关联 ID（Correlation ID）
 *   2. 将关联 ID 放入请求 Payload 中
 *   3. 订阅响应主题（包含关联 ID 过滤）
 *   4. 收到响应后根据关联 ID 匹配对应的请求
 *   5. 超时未收到响应的请求标记为"超时"
 */
class TabRequestResponse : public QWidget
{
    Q_OBJECT

public:
    explicit TabRequestResponse(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onSendRequest();      // 发送请求
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                           quint8 qos, bool retain, const QDateTime &timestamp); // 收到消息
    void onTimeoutCheck();     // 定时检查超时请求

private:
    void setupUI();
    void insertRequestRow(const QString &correlationId, const QString &topic,
                          const QString &payload, const QDateTime &time);
    void updateRequestRow(const QString &correlationId, const QDateTime &respTime,
                          const QString &respPayload, const QString &status);
    int findRowByCorrelationId(const QString &correlationId);

    MqttClientWrapper *m_wrapper;

    // 顶部配置区域
    QLineEdit    *m_requestTopicEdit;   // 请求主题输入框
    QLineEdit    *m_responseTopicEdit;   // 响应主题输入框
    QSpinBox     *m_timeoutSpin;        // 超时时间（秒）

    // 中部请求编辑区域
    QTextEdit     *m_requestPayloadEdit; // 请求内容编辑器
    QPushButton   *m_sendButton;         // 发送请求按钮

    // 底部请求-响应表格
    QTableWidget  *m_requestTable;       // 请求-响应记录表格

    // 超时检查定时器
    QTimer        *m_timeoutTimer;

    // 待响应请求映射：关联ID → 请求发送时间
    QMap<QString, QDateTime> m_pendingRequests;
};

#endif // TAB_REQUEST_RESPONSE_H
