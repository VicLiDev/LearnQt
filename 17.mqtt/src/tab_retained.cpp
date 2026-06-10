#include "tab_retained.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QGroupBox>

TabRetained::TabRetained(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_topicEdit(new QLineEdit(this))
    , m_payloadEdit(new QTextEdit(this))
    , m_retainCheckBox(new QCheckBox(tr("保留消息 (Retain)"), this))
    , m_publishBtn(new QPushButton(tr("发布"), this))
    , m_clearRetainedBtn(new QPushButton(tr("清除保留消息"), this))
    , m_subTopicEdit(new QLineEdit(this))
    , m_subscribeBtn(new QPushButton(tr("订阅"), this))
    , m_receivedEdit(new QTextEdit(this))
    , m_retainedTable(new QTableWidget(this))
    , m_waitingRetained(false)
{
    setupUi();
    setupConnections();
}

void TabRetained::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 说明区域 =====
    QLabel *descLabel = new QLabel(
        tr("Retained Message（保留消息）：\n"
           "当发布消息时设置 Retain 标志，Broker 会保存该消息。\n"
           "新的订阅者连接后，会立即收到该主题上最新的保留消息。\n"
           "发布空 payload 的保留消息可清除该主题上的保留消息。"),
        this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("QLabel { color: #666; font-style: italic; padding: 5px; }");
    mainLayout->addWidget(descLabel);

    // ===== 发布区域 =====
    QGroupBox *publishGroup = new QGroupBox(tr("发布消息"), this);
    QGridLayout *publishLayout = new QGridLayout(publishGroup);

    publishLayout->addWidget(new QLabel(tr("主题:"), this), 0, 0);
    m_topicEdit->setPlaceholderText(tr("例如: sensor/temperature"));
    publishLayout->addWidget(m_topicEdit, 0, 1);

    publishLayout->addWidget(new QLabel(tr("消息内容:"), this), 1, 0);
    m_payloadEdit->setPlaceholderText(tr("输入消息内容..."));
    m_payloadEdit->setMaximumHeight(60);
    publishLayout->addWidget(m_payloadEdit, 1, 1);

    publishLayout->addWidget(m_retainCheckBox, 2, 0, 1, 2);

    // 发布和清除按钮
    QHBoxLayout *pubBtnLayout = new QHBoxLayout();
    pubBtnLayout->addWidget(m_publishBtn);
    m_clearRetainedBtn->setToolTip(
        tr("向主题发布空 payload 的保留消息，以清除 Broker 上的保留消息"));
    m_clearRetainedBtn->setStyleSheet(
        "QPushButton { color: orange; font-weight: bold; }");
    pubBtnLayout->addWidget(m_clearRetainedBtn);
    publishLayout->addLayout(pubBtnLayout, 3, 0, 1, 2);

    mainLayout->addWidget(publishGroup);

    // ===== 订阅区域 =====
    QGroupBox *subscribeGroup = new QGroupBox(tr("订阅"), this);
    QHBoxLayout *subLayout = new QHBoxLayout(subscribeGroup);

    subLayout->addWidget(new QLabel(tr("主题过滤器:"), this));
    m_subTopicEdit->setPlaceholderText(tr("例如: sensor/#"));
    subLayout->addWidget(m_subTopicEdit);
    subLayout->addWidget(m_subscribeBtn);

    mainLayout->addWidget(subscribeGroup);

    // ===== 接收消息区域 =====
    QGroupBox *receivedGroup = new QGroupBox(tr("接收到的消息"), this);
    QVBoxLayout *receivedLayout = new QVBoxLayout(receivedGroup);

    m_receivedEdit->setReadOnly(true);
    m_receivedEdit->setPlaceholderText(
        tr("收到的消息会显示在此处...\n"
           "红色标记的为保留消息（订阅后立即收到的历史消息）"));
    receivedLayout->addWidget(m_receivedEdit);

    mainLayout->addWidget(receivedGroup);

    // ===== 保留消息存储表格 =====
    QGroupBox *storeGroup = new QGroupBox(tr("保留消息存储（Broker 端）"), this);
    QVBoxLayout *storeLayout = new QVBoxLayout(storeGroup);

    // 初始化表格：4 列（主题、内容、QoS、时间）
    m_retainedTable->setColumnCount(4);
    m_retainedTable->setHorizontalHeaderLabels(
        QStringList() << tr("主题") << tr("内容") << tr("QoS") << tr("时间"));
    m_retainedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_retainedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_retainedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    storeLayout->addWidget(m_retainedTable);

    mainLayout->addWidget(storeGroup);

    mainLayout->addStretch();
}

void TabRetained::setupConnections()
{
    // 发布按钮
    connect(m_publishBtn, &QPushButton::clicked,
            this, &TabRetained::onPublish);

    // 清除保留消息按钮
    connect(m_clearRetainedBtn, &QPushButton::clicked,
            this, &TabRetained::onClearRetained);

    // 订阅按钮
    connect(m_subscribeBtn, &QPushButton::clicked,
            this, &TabRetained::onSubscribe);

    // 监听收到的消息
    connect(m_wrapper, &MqttClientWrapper::messageReceived,
            this, &TabRetained::onMessageReceived);
}

void TabRetained::onPublish()
{
    QString topic = m_topicEdit->text().trimmed();
    QByteArray payload = m_payloadEdit->toPlainText().toUtf8();
    bool retain = m_retainCheckBox->isChecked();

    if (topic.isEmpty()) {
        m_receivedEdit->append(
            QString("[%1] <span style=\"color:red;\">错误：主题不能为空</span>")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        return;
    }

    // 发布消息
    m_wrapper->publish(topic, payload, 0, retain);

    m_receivedEdit->append(
        QString("[%1] <span style=\"color:green;\">已发布消息</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    m_receivedEdit->append(
        QString("  主题: %1, 保留: %2").arg(topic).arg(retain ? "是" : "否"));
}

void TabRetained::onClearRetained()
{
    // 清除保留消息：向该主题发布空 payload 的保留消息
    QString topic = m_topicEdit->text().trimmed();

    if (topic.isEmpty()) {
        m_receivedEdit->append(
            QString("[%1] <span style=\"color:red;\">错误：请先输入要清除的主题</span>")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        return;
    }

    // 发布空 payload + retain=true 来清除该主题的保留消息
    m_wrapper->publish(topic, QByteArray(), 0, true);

    m_receivedEdit->append(
        QString("[%1] <span style=\"color:orange;\">已清除主题 '%2' 的保留消息</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(topic));

    // 同时清除本地表格中对应主题的记录
    for (int row = m_retainedTable->rowCount() - 1; row >= 0; --row) {
        if (m_retainedTable->item(row, 0)->text() == topic) {
            m_retainedTable->removeRow(row);
        }
    }
}

void TabRetained::onSubscribe()
{
    QString topicFilter = m_subTopicEdit->text().trimmed();

    if (topicFilter.isEmpty()) {
        m_receivedEdit->append(
            QString("[%1] <span style=\"color:red;\">错误：订阅主题不能为空</span>")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        return;
    }

    // 标记：正在等待保留消息（订阅后立即收到的为保留消息）
    m_waitingRetained = true;

    m_wrapper->subscribe(topicFilter, 0);

    m_receivedEdit->append(
        QString("[%1] <span style=\"color:blue;\">已订阅: %2</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(topicFilter));
}

void TabRetained::onMessageReceived(const QString &topic, const QByteArray &payload,
                                    quint8 qos, bool retain, const QDateTime &timestamp)
{
    // 判断是否为保留消息：
    // 保留消息 = retain 标志为 true，或者是在订阅后立即收到的消息
    bool isRetained = retain;

    // 如果正在等待保留消息且这是订阅后的首次消息，标记为保留消息
    if (m_waitingRetained && isRetained) {
        m_waitingRetained = false;
    }

    QString timeStr = timestamp.toString("yyyy-MM-dd HH:mm:ss");
    QString payloadStr = QString::fromUtf8(payload);

    if (isRetained) {
        // 红色标记显示保留消息
        m_receivedEdit->append(
            QString("<span style=\"color:red; font-weight:bold;\">"
                    "[保留消息] %1</span>").arg(timeStr));
        m_receivedEdit->append(
            QString("  <span style=\"color:red;\">主题: %1</span>").arg(topic));
        m_receivedEdit->append(
            QString("  <span style=\"color:red;\">内容: %1</span>").arg(payloadStr));
    } else {
        // 普通消息
        m_receivedEdit->append(
            QString("[%1] <span style=\"color:green;\">主题: %2</span>")
                .arg(timeStr).arg(topic));
        m_receivedEdit->append(
            QString("  内容: %1").arg(payloadStr));
    }
    m_receivedEdit->append(
        QString("  QoS: %1").arg(qos));
    m_receivedEdit->append("---");

    // 如果是保留消息，更新本地表格
    if (isRetained && !payloadStr.isEmpty()) {
        int row = m_retainedTable->rowCount();
        m_retainedTable->insertRow(row);
        m_retainedTable->setItem(row, 0, new QTableWidgetItem(topic));
        m_retainedTable->setItem(row, 1, new QTableWidgetItem(payloadStr));
        m_retainedTable->setItem(row, 2, new QTableWidgetItem(QString::number(qos)));
        m_retainedTable->setItem(row, 3, new QTableWidgetItem(timeStr));

        // 空消息表示清除，不添加到表格（已在 onClearRetained 中处理）
    }
}
