#include "tab_qos_level.h"
#include "mqtt_client_wrapper.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QScrollBar>

// ============================================================
// 辅助函数：创建一个 QoS 发布面板
//
// 每个面板包含：主题输入、消息编辑、发布按钮、状态标签
// ============================================================

static QGroupBox *createQosPanel(const QString &title,
                                 QLineEdit *&outTopic,
                                 QTextEdit *&outMsg,
                                 QPushButton *&outBtn,
                                 QLabel *&outStatus)
{
    QGroupBox *group = new QGroupBox(title);
    QVBoxLayout *layout = new QVBoxLayout(group);

    // 主题输入
    layout->addWidget(new QLabel("主题:"));
    outTopic = new QLineEdit();
    outTopic->setPlaceholderText("qos/test");
    layout->addWidget(outTopic);

    // 消息内容
    layout->addWidget(new QLabel("消息:"));
    outMsg = new QTextEdit();
    outMsg->setMaximumHeight(80);
    outMsg->setPlaceholderText("输入测试消息...");
    layout->addWidget(outMsg);

    // 发布按钮
    outBtn = new QPushButton("发布");
    outBtn->setStyleSheet("QPushButton { padding: 6px; font-weight: bold; }");
    layout->addWidget(outBtn);

    // 状态标签 —— 用于展示消息投递状态变化
    outStatus = new QLabel("等待发布...");
    outStatus->setWordWrap(true);
    outStatus->setStyleSheet("QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; }");
    layout->addWidget(outStatus);

    layout->addStretch();

    return group;
}

// ============================================================
// 构造函数
// ============================================================

TabQosLevel::TabQosLevel(MqttClientWrapper *client, QWidget *parent)
    : QWidget(parent)
    , m_client(client)
    , m_editTopic0(nullptr)
    , m_editMsg0(nullptr)
    , m_btnPub0(nullptr)
    , m_labelStatus0(nullptr)
    , m_editTopic1(nullptr)
    , m_editMsg1(nullptr)
    , m_btnPub1(nullptr)
    , m_labelStatus1(nullptr)
    , m_editTopic2(nullptr)
    , m_editMsg2(nullptr)
    , m_btnPub2(nullptr)
    , m_labelStatus2(nullptr)
    , m_labelReceivedInfo(nullptr)
    , m_textReceived(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ============ 三个 QoS 面板并排显示 ============
    QHBoxLayout *panelLayout = new QHBoxLayout();

    // QoS 0：最多一次（At most once）
    // 发送端发出 PUBLISH 后不等待确认，消息可能丢失，但开销最小。
    QGroupBox *group0 = createQosPanel("QoS 0 — 最多一次\n（Fire and Forget）",
                                        m_editTopic0, m_editMsg0,
                                        m_btnPub0, m_labelStatus0);
    m_btnPub0->setStyleSheet("QPushButton { background-color: #8BC34A; color: white; padding: 6px; font-weight: bold; }");
    panelLayout->addWidget(group0);

    // QoS 1：至少一次（At least once）
    // 发送端发出 PUBLISH 后等待 PUBACK 确认，超时则重发，消息可能重复。
    QGroupBox *group1 = createQosPanel("QoS 1 — 至少一次\n（At least once）",
                                        m_editTopic1, m_editMsg1,
                                        m_btnPub1, m_labelStatus1);
    m_btnPub1->setStyleSheet("QPushButton { background-color: #FFC107; color: black; padding: 6px; font-weight: bold; }");
    panelLayout->addWidget(group1);

    // QoS 2：恰好一次（Exactly once）
    // 四步握手（PUBLISH → PUBREC → PUBREL → PUBCOMP），保证消息恰好送达一次，开销最大。
    QGroupBox *group2 = createQosPanel("QoS 2 — 恰好一次\n（Exactly once）",
                                        m_editTopic2, m_editMsg2,
                                        m_btnPub2, m_labelStatus2);
    m_btnPub2->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 6px; font-weight: bold; }");
    panelLayout->addWidget(group2);

    mainLayout->addLayout(panelLayout);

    // ============ QoS 对比说明 ============
    QLabel *labelCompare = new QLabel(
        "<b>QoS 对比表</b><br>"
        "<table border='1' cellpadding='4' cellspacing='0' style='border-collapse:collapse;'>"
        "<tr style='background:#f0f0f0;'>"
        "  <th>级别</th><th>名称</th><th>可靠性</th><th>开销</th><th>适用场景</th>"
        "</tr>"
        "<tr>"
        "  <td>QoS 0</td><td>最多一次</td><td>可能丢失</td><td>最小</td><td>传感器数据、日志</td>"
        "</tr>"
        "<tr>"
        "  <td>QoS 1</td><td>至少一次</td><td>可能重复</td><td>中等</td><td>消息通知、指令</td>"
        "</tr>"
        "<tr>"
        "  <td>QoS 2</td><td>恰好一次</td><td>保证唯一</td><td>最大</td><td>计费、关键控制</td>"
        "</tr>"
        "</table>");
    labelCompare->setWordWrap(true);
    mainLayout->addWidget(labelCompare);

    // ============ 消息接收区 ============
    QGroupBox *groupRecv = new QGroupBox("接收到的消息");
    QVBoxLayout *recvLayout = new QVBoxLayout(groupRecv);

    m_labelReceivedInfo = new QLabel("监听所有 QoS 级别的消息...");
    recvLayout->addWidget(m_labelReceivedInfo);

    m_textReceived = new QTextEdit();
    m_textReceived->setReadOnly(true);
    m_textReceived->setFont(QFont("Consolas", 9));
    m_textReceived->setMaximumHeight(120);
    recvLayout->addWidget(m_textReceived);

    mainLayout->addWidget(groupRecv);

    // ============ 信号连接 ============
    connect(m_btnPub0, &QPushButton::clicked, this, &TabQosLevel::publishQos0);
    connect(m_btnPub1, &QPushButton::clicked, this, &TabQosLevel::publishQos1);
    connect(m_btnPub2, &QPushButton::clicked, this, &TabQosLevel::publishQos2);

    // 监听 wrapper 转发的所有消息
    connect(m_client, &MqttClientWrapper::messageReceived,
            this, &TabQosLevel::onMessageReceived);
}

// ============================================================
// 以 QoS 0 发布 —— 最多一次（Fire and Forget）
// ============================================================

void TabQosLevel::publishQos0()
{
    QString topic = m_editTopic0->text().trimmed();
    if (topic.isEmpty()) {
        m_labelStatus0->setText("⚠ 请输入主题！");
        m_labelStatus0->setStyleSheet("QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; color: red; }");
        return;
    }

    QByteArray payload = m_editMsg0->toPlainText().toUtf8();
    quint32 msgId = m_client->publish(topic, payload, 0, false);

    // QoS 0 无确认机制，消息发出即视为完成
    m_labelStatus0->setText(QString("✔ 已发送 (msgId=%1)\n消息发出后不等待确认").arg(msgId));
    m_labelStatus0->setStyleSheet(
        "QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; "
        "background-color: #C8E6C9; }");  // 绿色背景 = 成功
}

// ============================================================
// 以 QoS 1 发布 —— 至少一次
// ============================================================

void TabQosLevel::publishQos1()
{
    QString topic = m_editTopic1->text().trimmed();
    if (topic.isEmpty()) {
        m_labelStatus1->setText("⚠ 请输入主题！");
        m_labelStatus1->setStyleSheet("QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; color: red; }");
        return;
    }

    QByteArray payload = m_editMsg1->toPlainText().toUtf8();
    quint32 msgId = m_client->publish(topic, payload, 1, false);

    // QoS 1 等待 PUBACK 确认，超时会重发
    m_labelStatus1->setText(QString("⏳ 已发送，等待 PUBACK (msgId=%1)\n"
                                     "Broker 确认后标记为已送达").arg(msgId));
    m_labelStatus1->setStyleSheet(
        "QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; "
        "background-color: #FFF9C4; }");  // 黄色背景 = 等待确认
}

// ============================================================
// 以 QoS 2 发布 —— 恰好一次
// ============================================================

void TabQosLevel::publishQos2()
{
    QString topic = m_editTopic2->text().trimmed();
    if (topic.isEmpty()) {
        m_labelStatus2->setText("⚠ 请输入主题！");
        m_labelStatus2->setStyleSheet("QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; color: red; }");
        return;
    }

    QByteArray payload = m_editMsg2->toPlainText().toUtf8();
    quint32 msgId = m_client->publish(topic, payload, 2, false);

    // QoS 2 使用四步握手：PUBLISH → PUBREC → PUBREL → PUBCOMP
    m_labelStatus2->setText(QString("🔄 四步握手中 (msgId=%1)\n"
                                     "PUBLISH → PUBREC → PUBREL → PUBCOMP").arg(msgId));
    m_labelStatus2->setStyleSheet(
        "QLabel { padding: 4px; border: 1px solid #ccc; border-radius: 3px; "
        "background-color: #BBDEFB; }");  // 蓝色背景 = 握手进行中
}

// ============================================================
// 收到消息 —— 在接收区显示，标注 QoS 级别
// ============================================================

void TabQosLevel::onMessageReceived(const QString &topic,
                                    const QByteArray &payload,
                                    quint8 qos, bool retain)
{
    // 根据不同 QoS 级别用不同颜色标注
    QString color;
    QString qosName;
    switch (qos) {
    case 0: color = "#8BC34A"; qosName = "QoS 0"; break;  // 绿色
    case 1: color = "#FFC107"; qosName = "QoS 1"; break;  // 黄色
    case 2: color = "#2196F3"; qosName = "QoS 2"; break;  // 蓝色
    default: color = "#999";    qosName = QString("QoS %1").arg(qos); break;
    }

    QString retainStr = retain ? " [Retain]" : "";
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    m_textReceived->append(
        QString("<span style='color:gray'>[%1]</span> "
                "<span style='color:%2;font-weight:bold'>[%3]</span> "
                "<b>%4</b>: %5%6")
            .arg(timestamp, color, qosName, topic,
                 QString::fromUtf8(payload), retainStr));

    m_textReceived->verticalScrollBar()->setValue(
        m_textReceived->verticalScrollBar()->maximum());
}
