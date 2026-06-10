#include "tab_publish_subscribe.h"
#include "mqtt_client_wrapper.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDateTime>
#include <QMessageBox>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>

// ============================================================
// 构造函数 —— 初始化 UI 并连接信号
// ============================================================

TabPublishSubscribe::TabPublishSubscribe(MqttClientWrapper *client, QWidget *parent)
    : QWidget(parent)
    , m_client(client)
    , m_editSubTopic(nullptr)
    , m_btnSubscribe(nullptr)
    , m_btnUnsubscribe(nullptr)
    , m_listSubscriptions(nullptr)
    , m_editPubTopic(nullptr)
    , m_editPayload(nullptr)
    , m_spinQos(nullptr)
    , m_checkRetain(nullptr)
    , m_btnPublish(nullptr)
    , m_tableHistory(nullptr)
    , m_btnClearHistory(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ============ 顶部：订阅区域 ============
    // MQTT 订阅：客户端向 Broker 发送 SUBSCRIBE 报文，
    // 指定一个主题过滤器（Topic Filter），Broker 会将匹配的消息推送过来。
    QGroupBox *groupSub = new QGroupBox("订阅主题");
    QHBoxLayout *subLayout = new QHBoxLayout(groupSub);

    m_editSubTopic = new QLineEdit();
    m_editSubTopic->setPlaceholderText("输入主题，例如: test/topic");

    m_btnSubscribe = new QPushButton("订阅");
    m_btnSubscribe->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 6px 16px; }");

    m_btnUnsubscribe = new QPushButton("取消订阅");
    m_btnUnsubscribe->setEnabled(false);

    subLayout->addWidget(new QLabel("主题:"));
    subLayout->addWidget(m_editSubTopic, 1);
    subLayout->addWidget(m_btnSubscribe);
    subLayout->addWidget(m_btnUnsubscribe);

    mainLayout->addWidget(groupSub);

    // ============ 中部：订阅列表 + 发布区域（左右布局） ============
    QHBoxLayout *middleLayout = new QHBoxLayout();

    // ---- 中部左侧：当前订阅列表 ----
    // 展示本 Tab 中已订阅的主题，方便管理和取消订阅
    QGroupBox *groupList = new QGroupBox("已订阅主题");
    QVBoxLayout *listLayout = new QVBoxLayout(groupList);

    m_listSubscriptions = new QListWidget();
    m_listSubscriptions->setMaximumWidth(250);
    listLayout->addWidget(m_listSubscriptions);

    middleLayout->addWidget(groupList);

    // ---- 中部右侧：发布区域 ----
    // MQTT 发布：客户端向 Broker 发送 PUBLISH 报文，
    // Broker 会将消息分发给所有订阅了匹配主题的客户端。
    QGroupBox *groupPub = new QGroupBox("发布消息");
    QGridLayout *pubLayout = new QGridLayout(groupPub);

    pubLayout->addWidget(new QLabel("主题:"), 0, 0);
    m_editPubTopic = new QLineEdit();
    m_editPubTopic->setPlaceholderText("输入发布主题，例如: test/topic");
    pubLayout->addWidget(m_editPubTopic, 0, 1);

    pubLayout->addWidget(new QLabel("内容:"), 1, 0);
    m_editPayload = new QTextEdit();
    m_editPayload->setMaximumHeight(100);
    m_editPayload->setPlaceholderText("输入消息内容...");
    pubLayout->addWidget(m_editPayload, 1, 1);

    // QoS 选择：控制消息传输的可靠等级
    //   0 = 最多一次（Fire and Forget）
    //   1 = 至少一次（至少到达一次，可能重复）
    //   2 = 恰好一次（保证只到达一次，开销最大）
    QHBoxLayout *optionLayout = new QHBoxLayout();

    optionLayout->addWidget(new QLabel("QoS:"));
    m_spinQos = new QSpinBox();
    m_spinQos->setRange(0, 2);
    m_spinQos->setValue(0);
    m_spinQos->setToolTip("0=最多一次, 1=至少一次, 2=恰好一次");
    optionLayout->addWidget(m_spinQos);

    m_checkRetain = new QCheckBox("Retain 保留");
    m_checkRetain->setToolTip("保留消息：Broker 会保存最后一条 Retain 消息，新订阅者会立即收到");
    optionLayout->addWidget(m_checkRetain);

    optionLayout->addStretch();

    m_btnPublish = new QPushButton("发布");
    m_btnPublish->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 6px 24px; font-weight: bold; }");
    optionLayout->addWidget(m_btnPublish);

    pubLayout->addLayout(optionLayout, 2, 0, 1, 2);

    middleLayout->addWidget(groupPub, 1);

    mainLayout->addLayout(middleLayout);

    // ============ 底部：消息历史表格 ============
    // 实时展示收到的所有 MQTT 消息，方便观察 Pub/Sub 效果
    QGroupBox *groupHistory = new QGroupBox("消息历史");
    QVBoxLayout *historyLayout = new QVBoxLayout(groupHistory);

    m_tableHistory = new QTableWidget(0, 5);
    m_tableHistory->setHorizontalHeaderLabels(
        QStringList() << "时间" << "主题" << "QoS" << "Retain" << "Payload");
    m_tableHistory->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableHistory->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableHistory->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableHistory->setAlternatingRowColors(true);

    m_btnClearHistory = new QPushButton("清空历史");
    historyLayout->addWidget(m_tableHistory, 1);
    historyLayout->addWidget(m_btnClearHistory, 0, Qt::AlignRight);

    mainLayout->addWidget(groupHistory, 1);

    // ============ 信号连接 ============
    connect(m_btnSubscribe, &QPushButton::clicked, this, &TabPublishSubscribe::onSubscribe);
    connect(m_btnUnsubscribe, &QPushButton::clicked, this, &TabPublishSubscribe::onUnsubscribe);
    connect(m_btnPublish, &QPushButton::clicked, this, &TabPublishSubscribe::onPublish);
    connect(m_btnClearHistory, &QPushButton::clicked, m_tableHistory, &QTableWidget::clearContents);
    connect(m_btnClearHistory, &QPushButton::clicked, [this]() {
        m_tableHistory->setRowCount(0);
    });

    // 监听 wrapper 转发的所有消息（无论是否由本 Tab 订阅）
    connect(m_client, &MqttClientWrapper::messageReceived,
            this, &TabPublishSubscribe::onMessageReceived);
}

// ============================================================
// 槽函数：订阅主题
// ============================================================

void TabPublishSubscribe::onSubscribe()
{
    QString topic = m_editSubTopic->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要订阅的主题！");
        return;
    }

    // 检查是否已订阅同一主题（避免重复订阅）
    for (int i = 0; i < m_listSubscriptions->count(); ++i) {
        if (m_listSubscriptions->item(i)->text() == topic) {
            QMessageBox::information(this, "提示", "该主题已在订阅列表中。");
            return;
        }
    }

    // 调用 wrapper 进行订阅，QoS 使用默认值 0
    // subscribe() 返回 QMqttSubscription*，后续可用于取消订阅
    quint8 qos = 0;
    QMqttSubscription *sub = m_client->subscribe(topic, qos);

    if (sub) {
        // 将订阅主题和指针对象存入列表，item 的 data 存指针便于取消订阅时使用
        QListWidgetItem *item = new QListWidgetItem(topic);
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<void *>(sub)));
        m_listSubscriptions->addItem(item);

        m_btnUnsubscribe->setEnabled(true);
    }
}

// ============================================================
// 槽函数：取消订阅
// ============================================================

void TabPublishSubscribe::onUnsubscribe()
{
    QListWidgetItem *item = m_listSubscriptions->currentItem();
    if (!item) {
        QMessageBox::information(this, "提示", "请先在列表中选择要取消的主题。");
        return;
    }

    QString topic = item->text();

    // 从 wrapper 取消订阅，Broker 不再推送匹配该主题的消息
    m_client->unsubscribe(topic);

    // 从列表中移除
    delete m_listSubscriptions->takeItem(m_listSubscriptions->row(item));

    // 如果列表为空，禁用取消订阅按钮
    if (m_listSubscriptions->count() == 0) {
        m_btnUnsubscribe->setEnabled(false);
    }
}

// ============================================================
// 槽函数：发布消息
// ============================================================

void TabPublishSubscribe::onPublish()
{
    QString topic = m_editPubTopic->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入发布主题！");
        return;
    }

    QByteArray payload = m_editPayload->toPlainText().toUtf8();
    quint8 qos = static_cast<quint8>(m_spinQos->value());
    bool retain = m_checkRetain->isChecked();

    // 调用 wrapper 发布消息到 Broker
    // publish() 返回消息 ID（Message ID），可用于追踪消息状态
    quint32 msgId = m_client->publish(topic, payload, qos, retain);

    Q_UNUSED(msgId);
}

// ============================================================
// 槽函数：收到消息 —— 在历史表格中插入一行
// ============================================================

void TabPublishSubscribe::onMessageReceived(const QString &topic,
                                            const QByteArray &payload,
                                            quint8 qos, bool retain)
{
    int row = m_tableHistory->rowCount();
    m_tableHistory->insertRow(row);

    // 时间列
    m_tableHistory->setItem(row, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("HH:mm:ss.zzz")));

    // 主题列
    m_tableHistory->setItem(row, 1, new QTableWidgetItem(topic));

    // QoS 列
    m_tableHistory->setItem(row, 2, new QTableWidgetItem(QString::number(qos)));

    // Retain 列 —— 标记是否为 Broker 保留的消息
    m_tableHistory->setItem(row, 3, new QTableWidgetItem(retain ? "是" : "否"));

    // Payload 列
    m_tableHistory->setItem(row, 4, new QTableWidgetItem(QString::fromUtf8(payload)));

    // 自动滚动到最新行
    m_tableHistory->scrollToBottom();
}
