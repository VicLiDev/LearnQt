#ifndef TAB_QOS_LEVEL_H
#define TAB_QOS_LEVEL_H

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;
class QSpinBox;
class MqttClientWrapper;

/**
 * @brief Tab 2: QoS 级别演示
 *
 * 并排展示 MQTT 三种 QoS（服务质量）级别的差异：
 *   - QoS 0：最多一次（At most once）—— Fire and Forget
 *   - QoS 1：至少一次（At least once）—— 有确认，可能重复
 *   - QoS 2：恰好一次（Exactly once）—— 四步握手，保证唯一
 * 每组可独立发布消息，并通过回调显示消息投递状态。
 */
class TabQosLevel : public QWidget
{
    Q_OBJECT

public:
    explicit TabQosLevel(MqttClientWrapper *client, QWidget *parent = nullptr);

private slots:
    void publishQos0();             // 以 QoS 0 发布消息
    void publishQos1();             // 以 QoS 1 发布消息
    void publishQos2();             // 以 QoS 2 发布消息
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                            quint8 qos, bool retain);   // 收到消息时显示

private:
    MqttClientWrapper *m_client;

    // ===== QoS 0 组控件 =====
    QLineEdit   *m_editTopic0;
    QTextEdit   *m_editMsg0;
    QPushButton *m_btnPub0;
    QLabel      *m_labelStatus0;

    // ===== QoS 1 组控件 =====
    QLineEdit   *m_editTopic1;
    QTextEdit   *m_editMsg1;
    QPushButton *m_btnPub1;
    QLabel      *m_labelStatus1;

    // ===== QoS 2 组控件 =====
    QLineEdit   *m_editTopic2;
    QTextEdit   *m_editMsg2;
    QPushButton *m_btnPub2;
    QLabel      *m_labelStatus2;

    // ===== 底部消息接收区 =====
    QLabel     *m_labelReceivedInfo;
    QTextEdit  *m_textReceived;
};

#endif // TAB_QOS_LEVEL_H
