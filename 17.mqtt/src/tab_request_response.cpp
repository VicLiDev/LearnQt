#include "tab_request_response.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QMessageBox>
#include <QGroupBox>
#include <QBrush>

// ========== 默认配置 ==========
static const QString kDefaultRequestTopic  = "qt/request/response";
static const QString kDefaultResponseTopic = "qt/response/response";
static const int     kDefaultTimeoutSec   = 10;
static const int     kTimeoutCheckIntervalMs = 1000; // 超时检查间隔：1秒

TabRequestResponse::TabRequestResponse(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_timeoutTimer(new QTimer(this))
{
    setupUI();

    // 超时检查定时器：每秒检查一次是否有请求超时
    m_timeoutTimer->setInterval(kTimeoutCheckIntervalMs);
    connect(m_timeoutTimer, &QTimer::timeout,
            this, &TabRequestResponse::onTimeoutCheck);

    // 监听 wrapper 的消息接收信号
    connect(m_wrapper, &MqttClientWrapper::messageReceived,
            this, &TabRequestResponse::onMessageReceived);
}

void TabRequestResponse::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 顶部：主题配置区域 =====
    QGroupBox *configGroup = new QGroupBox(tr("主题配置"));
    QGridLayout *configGrid = new QGridLayout(configGroup);

    configGrid->addWidget(new QLabel(tr("请求主题:")), 0, 0);
    m_requestTopicEdit = new QLineEdit(kDefaultRequestTopic);
    configGrid->addWidget(m_requestTopicEdit, 0, 1);

    configGrid->addWidget(new QLabel(tr("响应主题:")), 0, 2);
    m_responseTopicEdit = new QLineEdit(kDefaultResponseTopic);
    configGrid->addWidget(m_responseTopicEdit, 0, 3);

    configGrid->addWidget(new QLabel(tr("超时(秒):")), 0, 4);
    m_timeoutSpin = new QSpinBox();
    m_timeoutSpin->setRange(1, 300);
    m_timeoutSpin->setValue(kDefaultTimeoutSec);
    configGrid->addWidget(m_timeoutSpin, 0, 5);

    mainLayout->addWidget(configGroup);

    // ===== 中部：请求编辑区域 =====
    QGroupBox *editGroup = new QGroupBox(tr("请求内容"));
    QVBoxLayout *editLayout = new QVBoxLayout(editGroup);

    m_requestPayloadEdit = new QTextEdit();
    m_requestPayloadEdit->setPlaceholderText(
        tr("输入请求内容（JSON 格式），发送时会自动附加关联ID"));
    m_requestPayloadEdit->setMaximumHeight(120);
    editLayout->addWidget(m_requestPayloadEdit);

    m_sendButton = new QPushButton(tr("发送请求"));
    connect(m_sendButton, &QPushButton::clicked,
            this, &TabRequestResponse::onSendRequest);
    editLayout->addWidget(m_sendButton, 0, Qt::AlignRight);

    mainLayout->addWidget(editGroup);

    // ===== 底部：请求-响应表格 =====
    QGroupBox *tableGroup = new QGroupBox(tr("请求-响应记录"));
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    m_requestTable = new QTableWidget(0, 7);
    m_requestTable->setHorizontalHeaderLabels(
        QStringList() << tr("ID") << tr("请求时间") << tr("请求主题")
                      << tr("请求内容") << tr("响应时间") << tr("响应内容")
                      << tr("状态"));
    m_requestTable->horizontalHeader()->setStretchLastSection(true);
    m_requestTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_requestTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_requestTable->setAlternatingRowColors(true);

    // 设置列宽
    m_requestTable->setColumnWidth(0, 80);   // ID（关联ID缩略）
    m_requestTable->setColumnWidth(1, 160);  // 请求时间
    m_requestTable->setColumnWidth(2, 160);  // 请求主题
    m_requestTable->setColumnWidth(4, 160);  // 响应时间
    m_requestTable->setColumnWidth(6, 80);   // 状态

    tableLayout->addWidget(m_requestTable);
    mainLayout->addWidget(tableGroup);

    // 说明文字
    QLabel *tipLabel = new QLabel(
        tr("说明：MQTT 是发布/订阅模型，没有原生的请求-响应语义。"
           "本模式通过关联 ID（Correlation ID）模拟 RPC 调用："
           "发送请求时附带唯一 ID，响应中携带相同 ID 以匹配对应的请求。"));
    tipLabel->setWordWrap(true);
    tipLabel->setStyleSheet("QLabel { color: gray; font-size: 11px; }");
    mainLayout->addWidget(tipLabel);
}

void TabRequestResponse::onSendRequest()
{
    // 检查连接状态
    if (!m_wrapper->isConnected()) {
        QMessageBox::warning(this, tr("未连接"),
                             tr("请先连接到 MQTT Broker 后再发送请求。"));
        return;
    }

    // 获取用户输入
    QString requestTopic  = m_requestTopicEdit->text().trimmed();
    QString responseTopic = m_responseTopicEdit->text().trimmed();
    QString payload       = m_requestPayloadEdit->toPlainText().trimmed();

    if (requestTopic.isEmpty() || responseTopic.isEmpty()) {
        QMessageBox::warning(this, tr("输入错误"),
                             tr("请求主题和响应主题不能为空。"));
        return;
    }

    // 生成唯一关联 ID
    QString correlationId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // 将关联 ID 附加到 Payload 中（JSON 格式）
    QJsonObject payloadObj;
    payloadObj["correlationId"] = correlationId;
    payloadObj["data"] = payload;
    payloadObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QByteArray finalPayload = QJsonDocument(payloadObj).toJson(QJsonDocument::Compact);

    // 订阅响应主题（过滤包含关联 ID 的响应）
    // 使用 MQTT 5 的共享订阅或通配符匹配关联 ID
    QString responseFilter = responseTopic + "/" + correlationId;
    m_wrapper->subscribe(responseFilter, 1);

    // 记录发送时间
    QDateTime sendTime = QDateTime::currentDateTime();

    // 存入待响应映射
    m_pendingRequests.insert(correlationId, sendTime);

    // 在表格中插入新行
    insertRequestRow(correlationId, requestTopic, payload, sendTime);

    // 启动超时检查定时器（如果尚未启动）
    if (!m_timeoutTimer->isActive()) {
        m_timeoutTimer->start();
    }
}

void TabRequestResponse::onMessageReceived(const QString &topic,
                                           const QByteArray &payload,
                                           quint8 qos, bool retain,
                                           const QDateTime &timestamp)
{
    Q_UNUSED(qos);
    Q_UNUSED(retain);
    Q_UNUSED(timestamp);

    // 尝试解析响应中的关联 ID
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        return; // 非 JSON 格式，忽略
    }

    QJsonObject obj = doc.object();
    QString correlationId = obj.value("correlationId").toString();

    if (correlationId.isEmpty() || !m_pendingRequests.contains(correlationId)) {
        return; // 没有关联 ID 或不匹配任何待响应请求
    }

    // 从待响应映射中移除（已收到响应）
    m_pendingRequests.remove(correlationId);

    // 提取响应数据
    QString responseData = obj.value("data").toString();
    QDateTime respTime = QDateTime::currentDateTime();

    // 更新表格中对应行的响应信息
    updateRequestRow(correlationId, respTime, responseData, tr("已完成"));
}

void TabRequestResponse::onTimeoutCheck()
{
    QDateTime now = QDateTime::currentDateTime();
    int timeoutSec = m_timeoutSpin->value();

    // 遍历所有待响应请求，检查是否超时
    QList<QString> expiredIds;
    for (auto it = m_pendingRequests.constBegin();
         it != m_pendingRequests.constEnd(); ++it) {
        if (it.value().secsTo(now) >= timeoutSec) {
            expiredIds.append(it.key());
        }
    }

    // 将超时的请求标记为"超时"并从映射中移除
    foreach (const QString &id, expiredIds) {
        m_pendingRequests.remove(id);
        updateRequestRow(id, QDateTime(), QString(), tr("超时"));
    }

    // 如果没有待响应请求了，停止超时检查定时器
    if (m_pendingRequests.isEmpty()) {
        m_timeoutTimer->stop();
    }
}

void TabRequestResponse::insertRequestRow(const QString &correlationId,
                                           const QString &topic,
                                           const QString &payload,
                                           const QDateTime &time)
{
    int row = m_requestTable->rowCount();
    m_requestTable->insertRow(row);

    // ID 列：显示关联 ID（缩略显示前 8 位）
    m_requestTable->setItem(row, 0, new QTableWidgetItem(correlationId.left(8) + "..."));

    // 请求时间
    m_requestTable->setItem(row, 1, new QTableWidgetItem(time.toString("HH:mm:ss.zzz")));

    // 请求主题
    m_requestTable->setItem(row, 2, new QTableWidgetItem(topic));

    // 请求内容（截断过长内容）
    QString displayPayload = payload.length() > 100
        ? payload.left(100) + "..." : payload;
    m_requestTable->setItem(row, 3, new QTableWidgetItem(displayPayload));

    // 响应时间、响应内容初始为空
    m_requestTable->setItem(row, 4, new QTableWidgetItem("-"));
    m_requestTable->setItem(row, 5, new QTableWidgetItem("-"));

    // 状态：等待响应
    QTableWidgetItem *statusItem = new QTableWidgetItem(tr("等待响应"));
    statusItem->setForeground(QBrush(Qt::darkYellow));
    m_requestTable->setItem(row, 6, statusItem);

    // 滚动到最新行
    m_requestTable->scrollToBottom();
}

void TabRequestResponse::updateRequestRow(const QString &correlationId,
                                           const QDateTime &respTime,
                                           const QString &respPayload,
                                           const QString &status)
{
    int row = findRowByCorrelationId(correlationId);
    if (row < 0) {
        return; // 未找到对应行
    }

    // 更新响应时间
    if (respTime.isValid()) {
        m_requestTable->item(row, 4)->setText(respTime.toString("HH:mm:ss.zzz"));
    }

    // 更新响应内容
    if (!respPayload.isEmpty()) {
        QString displayResp = respPayload.length() > 100
            ? respPayload.left(100) + "..." : respPayload;
        m_requestTable->item(row, 5)->setText(displayResp);
    }

    // 更新状态（根据状态文字设置不同颜色）
    QTableWidgetItem *statusItem = m_requestTable->item(row, 6);
    statusItem->setText(status);
    if (status == tr("已完成")) {
        statusItem->setForeground(QBrush(Qt::darkGreen));
    } else if (status == tr("超时")) {
        statusItem->setForeground(QBrush(Qt::red));
    }
}

int TabRequestResponse::findRowByCorrelationId(const QString &correlationId)
{
    // 在表格中查找匹配关联 ID 的行
    QString shortId = correlationId.left(8) + "...";
    for (int i = 0; i < m_requestTable->rowCount(); ++i) {
        QTableWidgetItem *item = m_requestTable->item(i, 0);
        if (item && item->text() == shortId) {
            return i;
        }
    }
    return -1;
}
