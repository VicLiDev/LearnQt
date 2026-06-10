#ifndef TAB_PUBLISH_SUBSCRIBE_H
#define TAB_PUBLISH_SUBSCRIBE_H

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QListWidget;
class QTableWidget;
class QSpinBox;
class QPushButton;
class QCheckBox;
class MqttClientWrapper;

/**
 * @brief Tab 1: 基础发布/订阅演示
 *
 * 演示 MQTT 最核心的发布/订阅模型：
 *   - 订阅主题（Topic），接收匹配的消息
 *   - 向指定主题发布消息（Payload）
 *   - 消息历史表格实时展示收到的消息
 */
class TabPublishSubscribe : public QWidget
{
    Q_OBJECT

public:
    explicit TabPublishSubscribe(MqttClientWrapper *client, QWidget *parent = nullptr);

private slots:
    void onSubscribe();       // 订阅主题
    void onUnsubscribe();     // 取消订阅
    void onPublish();         // 发布消息
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                            quint8 qos, bool retain);   // 收到消息

private:
    MqttClientWrapper *m_client;

    // 顶部：订阅区域
    QLineEdit   *m_editSubTopic;      // 订阅主题输入框
    QPushButton *m_btnSubscribe;      // 订阅按钮
    QPushButton *m_btnUnsubscribe;    // 取消订阅按钮

    // 中部左侧：订阅列表
    QListWidget *m_listSubscriptions; // 当前订阅列表

    // 中部右侧：发布区域
    QLineEdit    *m_editPubTopic;     // 发布主题输入框
    QTextEdit     *m_editPayload;     // 消息内容编辑器
    QSpinBox      *m_spinQos;          // QoS 级别选择
    QCheckBox     *m_checkRetain;     // Retain 标志
    QPushButton   *m_btnPublish;      // 发布按钮

    // 底部：消息历史
    QTableWidget *m_tableHistory;     // 消息历史表格
    QPushButton  *m_btnClearHistory;  // 清空历史按钮
};

#endif // TAB_PUBLISH_SUBSCRIBE_H
