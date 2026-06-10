#ifndef TAB_LWT_H
#define TAB_LWT_H

#include <QWidget>

class MqttClientWrapper;
class QLineEdit;
class QTextEdit;
class QSpinBox;
class QCheckBox;
class QPushButton;

/**
 * @brief Tab 4: Last Will 遗嘱消息
 *
 * 用于配置和测试 MQTT LWT（遗嘱消息）功能。
 * LWT 会在客户端异常断开时由 Broker 自动发布预设的消息。
 */
class TabLwt : public QWidget
{
    Q_OBJECT

public:
    explicit TabLwt(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onApplyWill();
    void onForceDisconnect();
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                           quint8 qos, bool retain, const QDateTime &timestamp);

private:
    void setupUi();
    void setupConnections();

    MqttClientWrapper *m_wrapper;

    // 遗嘱配置区域
    QLineEdit    *m_topicEdit;      // 遗嘱主题
    QTextEdit     *m_messageEdit;    // 遗嘱消息内容
    QSpinBox      *m_qosSpinBox;     // 遗嘱 QoS 等级
    QCheckBox     *m_retainCheckBox; // 遗嘱消息保留标志
    QPushButton   *m_applyButton;    // 应用 LWT 配置按钮
    QPushButton   *m_disconnectBtn;  // 模拟意外断开按钮

    // 接收消息显示区域
    QTextEdit     *m_receivedEdit;   // 显示收到的遗嘱消息
};

#endif // TAB_LWT_H
