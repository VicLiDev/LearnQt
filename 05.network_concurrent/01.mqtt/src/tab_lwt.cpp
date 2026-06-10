#include "tab_lwt.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDateTime>
#include <QGroupBox>

TabLwt::TabLwt(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_topicEdit(new QLineEdit(this))
    , m_messageEdit(new QTextEdit(this))
    , m_qosSpinBox(new QSpinBox(this))
    , m_retainCheckBox(new QCheckBox(tr("保留消息 (Retain)"), this))
    , m_applyButton(new QPushButton(tr("应用 LWT 配置"), this))
    , m_disconnectBtn(new QPushButton(tr("模拟意外断开"), this))
    , m_receivedEdit(new QTextEdit(this))
{
    setupUi();
    setupConnections();
}

void TabLwt::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 说明区域 =====
    QLabel *descLabel = new QLabel(
        tr("LWT（Last Will and Testament）遗嘱消息：\n"
           "当客户端异常断开连接时（例如网络故障、进程崩溃），\n"
           "Broker 会自动向指定的遗嘱主题发布预设的消息。\n"
           "正常调用 disconnectFromBroker() 不会触发遗嘱消息。"),
        this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("QLabel { color: #666; font-style: italic; padding: 5px; }");
    mainLayout->addWidget(descLabel);

    // ===== LWT 配置组 =====
    QGroupBox *configGroup = new QGroupBox(tr("LWT 配置"), this);
    QGridLayout *configLayout = new QGridLayout(configGroup);

    configLayout->addWidget(new QLabel(tr("遗嘱主题:"), this), 0, 0);
    m_topicEdit->setPlaceholderText(tr("例如: client/status/lwt"));
    configLayout->addWidget(m_topicEdit, 0, 1);

    configLayout->addWidget(new QLabel(tr("遗嘱消息:"), this), 1, 0);
    m_messageEdit->setPlaceholderText(tr("例如: 客户端已离线"));
    m_messageEdit->setMaximumHeight(80);
    configLayout->addWidget(m_messageEdit, 1, 1);

    configLayout->addWidget(new QLabel(tr("QoS:"), this), 2, 0);
    m_qosSpinBox->setRange(0, 2);
    m_qosSpinBox->setValue(0);
    configLayout->addWidget(m_qosSpinBox, 2, 1);

    configLayout->addWidget(m_retainCheckBox, 3, 0, 1, 2);

    // 操作按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_applyButton);
    m_disconnectBtn->setStyleSheet("QPushButton { color: red; font-weight: bold; }");
    btnLayout->addWidget(m_disconnectBtn);
    configLayout->addLayout(btnLayout, 4, 0, 1, 2);

    mainLayout->addWidget(configGroup);

    // ===== 接收到的遗嘱消息 =====
    QGroupBox *receivedGroup = new QGroupBox(tr("接收到的遗嘱消息"), this);
    QVBoxLayout *receivedLayout = new QVBoxLayout(receivedGroup);

    m_receivedEdit->setReadOnly(true);
    m_receivedEdit->setPlaceholderText(
        tr("此处显示收到的遗嘱消息...\n"
           "提示：应用 LWT 配置后会自动订阅遗嘱主题"));
    receivedLayout->addWidget(m_receivedEdit);

    mainLayout->addWidget(receivedGroup);

    mainLayout->addStretch();
}

void TabLwt::setupConnections()
{
    // 应用 LWT 配置按钮
    connect(m_applyButton, &QPushButton::clicked,
            this, &TabLwt::onApplyWill);

    // 模拟意外断开按钮
    connect(m_disconnectBtn, &QPushButton::clicked,
            this, &TabLwt::onForceDisconnect);

    // 监听收到的消息（用于显示遗嘱消息）
    connect(m_wrapper, &MqttClientWrapper::messageReceived,
            this, &TabLwt::onMessageReceived);
}

void TabLwt::onApplyWill()
{
    // 获取用户输入的配置
    QString topic = m_topicEdit->text().trimmed();
    QByteArray message = m_messageEdit->toPlainText().toUtf8();
    quint8 qos = static_cast<quint8>(m_qosSpinBox->value());
    bool retain = m_retainCheckBox->isChecked();

    // 验证主题是否为空
    if (topic.isEmpty()) {
        m_receivedEdit->append(
            QString("[%1] <span style=\"color:red;\">错误：遗嘱主题不能为空</span>")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        return;
    }

    // 将配置写入 wrapper
    m_wrapper->setWillTopic(topic);
    m_wrapper->setWillMessage(message);
    m_wrapper->setWillQoS(qos);
    m_wrapper->setWillRetain(retain);

    // 自动订阅遗嘱主题，以便接收 Broker 发出的遗嘱消息
    m_wrapper->subscribe(topic, qos);

    // 记录配置信息
    m_receivedEdit->append(
        QString("[%1] <span style=\"color:green;\">LWT 配置已应用</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    m_receivedEdit->append(
        QString("  主题: %1").arg(topic));
    m_receivedEdit->append(
        QString("  消息: %1").arg(QString::fromUtf8(message)));
    m_receivedEdit->append(
        QString("  QoS: %1, 保留: %2").arg(qos).arg(retain ? "是" : "否"));
    m_receivedEdit->append(
        QString("  已自动订阅主题: %1").arg(topic));
}

void TabLwt::onForceDisconnect()
{
    // 模拟意外断开：直接断开底层连接，不发送 DISCONNECT 报文
    // 这样 Broker 会认为客户端异常掉线，从而发布遗嘱消息
    QMqttClient *client = m_wrapper->client();
    if (client) {
        client->disconnectFromHost();
    }

    m_receivedEdit->append(
        QString("[%1] <span style=\"color:orange;\">已发送模拟意外断开...</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
}

void TabLwt::onMessageReceived(const QString &topic, const QByteArray &payload,
                               quint8 qos, bool retain, const QDateTime &timestamp)
{
    // 只显示遗嘱主题的消息
    QString willTopic = m_topicEdit->text().trimmed();
    if (willTopic.isEmpty()) {
        return;
    }

    if (topic != willTopic) {
        return;
    }

    // 使用醒目的红色标记显示收到的遗嘱消息
    QString timeStr = timestamp.toString("yyyy-MM-dd HH:mm:ss");
    m_receivedEdit->append(
        QString("<span style=\"color:red; font-weight:bold;\">"
                "[遗嘱消息] %1</span>").arg(timeStr));
    m_receivedEdit->append(
        QString("  <span style=\"color:red;\">主题: %1</span>").arg(topic));
    m_receivedEdit->append(
        QString("  <span style=\"color:red;\">内容: %1</span>")
            .arg(QString::fromUtf8(payload)));
    m_receivedEdit->append(
        QString("  QoS: %1, 保留: %2").arg(qos).arg(retain ? "是" : "否"));
    m_receivedEdit->append("---");
}
