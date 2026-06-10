#ifndef TAB_RETAINED_H
#define TAB_RETAINED_H

#include <QWidget>

class MqttClientWrapper;
class QLineEdit;
class QTextEdit;
class QCheckBox;
class QPushButton;
class QTableWidget;

/**
 * @brief Tab 5: 保留消息
 *
 * 用于演示 MQTT Retained Message 功能。
 * 保留消息会存储在 Broker 上，新订阅者会立即收到最新的保留消息。
 */
class TabRetained : public QWidget
{
    Q_OBJECT

public:
    explicit TabRetained(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onPublish();
    void onClearRetained();
    void onSubscribe();
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                           quint8 qos, bool retain, const QDateTime &timestamp);

private:
    void setupUi();
    void setupConnections();

    MqttClientWrapper *m_wrapper;

    // 发布区域
    QLineEdit    *m_topicEdit;       // 主题
    QTextEdit     *m_payloadEdit;     // 消息内容
    QCheckBox     *m_retainCheckBox; // 保留标志
    QPushButton   *m_publishBtn;      // 发布按钮
    QPushButton   *m_clearRetainedBtn; // 清除保留消息按钮

    // 订阅区域
    QLineEdit    *m_subTopicEdit;     // 订阅主题过滤器
    QPushButton   *m_subscribeBtn;    // 订阅按钮

    // 消息显示区域
    QTextEdit     *m_receivedEdit;   // 收到的消息显示
    QTableWidget  *m_retainedTable;   // 保留消息存储表格

    // 内部状态
    bool m_waitingRetained; // 是否正在等待订阅后的首次保留消息
};

#endif // TAB_RETAINED_H
