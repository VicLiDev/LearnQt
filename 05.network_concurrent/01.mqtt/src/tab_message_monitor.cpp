#include "tab_message_monitor.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QTimer>
#include <QDateTime>
#include <QScrollBar>
#include <QFrame>
#include <QBrush>
#include <QColor>
#include <QTextCursor>

// ========== 常量配置 ==========
static const int kStatsRefreshIntervalMs = 1000; // 统计刷新间隔：1秒

TabMessageMonitor::TabMessageMonitor(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_statsTimer(new QTimer(this))
    , m_totalPublished(0)
    , m_totalReceived(0)
    , m_lastPublished(0)
    , m_lastReceived(0)
    , m_publishRate(0.0)
    , m_subscribeRate(0.0)
    , m_connected(false)
{
    setupUI();

    // 统计刷新定时器：每秒更新一次速率
    m_statsTimer->setInterval(kStatsRefreshIntervalMs);
    m_statsTimer->start();
    connect(m_statsTimer, &QTimer::timeout,
            this, &TabMessageMonitor::onStatsRefresh);

    // 监听 wrapper 信号
    connect(m_wrapper, &MqttClientWrapper::messageReceived,
            this, &TabMessageMonitor::onMessageReceived);
    connect(m_wrapper, &MqttClientWrapper::messagePublished,
            this, &TabMessageMonitor::onMessagePublished);
    connect(m_wrapper, &MqttClientWrapper::publishFailed,
            this, &TabMessageMonitor::onPublishFailed);
    connect(m_wrapper, &MqttClientWrapper::connectionStateChanged,
            this, &TabMessageMonitor::onConnectionStateChanged);
}

void TabMessageMonitor::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 顶部：统计面板 =====
    QHBoxLayout *statsLayout = new QHBoxLayout();

    // 发布总数卡片（绿色）
    m_publishCard = createStatCard(tr("已发布"), "0", "#4CAF50");
    m_totalPublishLabel = qobject_cast<QLabel *>(
        m_publishCard->findChild<QLabel *>("statValueLabel"));
    statsLayout->addWidget(m_publishCard);

    // 接收总数卡片（蓝色）
    m_subscribeCard = createStatCard(tr("已接收"), "0", "#2196F3");
    m_totalSubscribeLabel = qobject_cast<QLabel *>(
        m_subscribeCard->findChild<QLabel *>("statValueLabel"));
    statsLayout->addWidget(m_subscribeCard);

    // 发布速率卡片（橙色）
    m_publishRateCard = createStatCard(tr("发布速率"), "0 条/秒", "#FF9800");
    m_publishRateLabel = qobject_cast<QLabel *>(
        m_publishRateCard->findChild<QLabel *>("statValueLabel"));
    statsLayout->addWidget(m_publishRateCard);

    // 接收速率卡片（紫色）
    m_subscribeRateCard = createStatCard(tr("接收速率"), "0 条/秒", "#9C27B0");
    m_subscribeRateLabel = qobject_cast<QLabel *>(
        m_subscribeRateCard->findChild<QLabel *>("statValueLabel"));
    statsLayout->addWidget(m_subscribeRateCard);

    // 连接时长卡片（灰色）
    m_durationCard = createStatCard(tr("连接时长"), "-", "#607D8B");
    m_connectionDurationLabel = qobject_cast<QLabel *>(
        m_durationCard->findChild<QLabel *>("statValueLabel"));
    statsLayout->addWidget(m_durationCard);

    statsLayout->addStretch();
    mainLayout->addLayout(statsLayout);

    // ===== 中部：最近消息表格 =====
    m_recentMessages = new QTableWidget(0, 5);
    m_recentMessages->setHorizontalHeaderLabels(
        QStringList() << tr("时间") << tr("方向") << tr("主题")
                       << tr("内容") << tr("QoS"));
    m_recentMessages->horizontalHeader()->setStretchLastSection(true);
    m_recentMessages->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_recentMessages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentMessages->setAlternatingRowColors(true);

    // 设置列宽
    m_recentMessages->setColumnWidth(0, 120); // 时间
    m_recentMessages->setColumnWidth(1, 50);  // 方向
    m_recentMessages->setColumnWidth(2, 200); // 主题
    m_recentMessages->setColumnWidth(4, 40); // QoS

    mainLayout->addWidget(m_recentMessages, 1); // stretch=1 占更多空间

    // ===== 底部：主题订阅树 =====
    QLabel *treeTitleLabel = new QLabel(tr("主题订阅树（自动构建）"));
    treeTitleLabel->setStyleSheet("QLabel { font-weight: bold; }");
    mainLayout->addWidget(treeTitleLabel);

    m_topicTree = new QTextEdit();
    m_topicTree->setReadOnly(true);
    m_topicTree->setMaximumHeight(150);
    m_topicTree->setPlaceholderText(tr("收到消息后自动构建主题层级结构..."));
    mainLayout->addWidget(m_topicTree);
}

QWidget *TabMessageMonitor::createStatCard(const QString &title,
                                             const QString &value,
                                             const QString &color)
{
    // 创建带背景色的统计卡片
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setStyleSheet(QString(
        "QFrame#statCard {"
        "  background-color: %1;"
        "  border-radius: 8px;"
        "  padding: 10px;"
        "  min-width: 120px;"
        "  min-height: 60px;"
        "}"
        "QLabel { color: white; }"
    ).arg(color));

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(10, 8, 10, 8);

    // 标题标签
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 12px; }");
    cardLayout->addWidget(titleLabel);

    // 数值标签
    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("statValueLabel");
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; }");
    cardLayout->addWidget(valueLabel);

    return card;
}

void TabMessageMonitor::updateStatCard(QWidget *card, const QString &value)
{
    QLabel *label = card->findChild<QLabel *>("statValueLabel");
    if (label) {
        label->setText(value);
    }
}

void TabMessageMonitor::onMessageReceived(const QString &topic,
                                           const QByteArray &payload,
                                           quint8 qos, bool retain,
                                           const QDateTime &timestamp)
{
    Q_UNUSED(retain);
    Q_UNUSED(timestamp);

    // 更新接收计数
    m_totalReceived++;

    // 添加到最近消息表格
    QString payloadStr = QString::fromUtf8(payload);
    addMessageToTable(tr("接收"), topic, payloadStr, qos);

    // 构建主题树
    addTopicToTree(topic);
}

void TabMessageMonitor::onMessagePublished(quint32 msgId, const QString &topic,
                                           const QByteArray &payload, quint8 qos)
{
    Q_UNUSED(msgId);

    // 更新发布计数
    m_totalPublished++;

    // 添加到最近消息表格
    QString payloadStr = QString::fromUtf8(payload);
    addMessageToTable(tr("发布"), topic, payloadStr, qos);
}

void TabMessageMonitor::onPublishFailed(quint32 msgId, const QString &topic,
                                          const QString &error)
{
    Q_UNUSED(msgId);

    // 发布失败也记录到表格
    addMessageToTable(tr("失败"), topic, error, 0);
}

void TabMessageMonitor::onConnectionStateChanged(bool connected)
{
    m_connected = connected;
    if (connected) {
        // 记录连接开始时间
        m_connectedSince = QDateTime::currentDateTime();
    }
}

void TabMessageMonitor::onStatsRefresh()
{
    // 使用简单增量法计算速率：当前总数 - 上次总数 = 本秒增量
    m_publishRate = m_totalPublished - m_lastPublished;
    m_subscribeRate = m_totalReceived - m_lastReceived;

    // 保存当前计数作为下次的基准
    m_lastPublished = m_totalPublished;
    m_lastReceived = m_totalReceived;

    // 更新统计卡片
    updateStatCard(m_publishCard,
                   QString::number(m_totalPublished));
    updateStatCard(m_subscribeCard,
                   QString::number(m_totalReceived));
    updateStatCard(m_publishRateCard,
                   QString("%1 条/秒").arg(m_publishRate, 0, 'f', 0));
    updateStatCard(m_subscribeRateCard,
                   QString("%1 条/秒").arg(m_subscribeRate, 0, 'f', 0));

    // 更新连接时长
    if (m_connected && m_connectedSince.isValid()) {
        qint64 secs = m_connectedSince.secsTo(QDateTime::currentDateTime());
        int hours = secs / 3600;
        int minutes = (secs % 3600) / 60;
        int seconds = secs % 60;
        QString duration;
        if (hours > 0) {
            duration = QString("%1时%2分%3秒").arg(hours).arg(minutes).arg(seconds);
        } else if (minutes > 0) {
            duration = QString("%1分%2秒").arg(minutes).arg(seconds);
        } else {
            duration = QString("%1秒").arg(seconds);
        }
        updateStatCard(m_durationCard, duration);
    } else {
        updateStatCard(m_durationCard, tr("未连接"));
    }
}

void TabMessageMonitor::addMessageToTable(const QString &direction,
                                            const QString &topic,
                                            const QString &payload,
                                            quint8 qos)
{
    // 在表格顶部插入新行（最新消息在最上方）
    m_recentMessages->insertRow(0);

    // 时间
    m_recentMessages->setItem(0, 0,
        new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")));

    // 方向（发布/接收/失败）
    QTableWidgetItem *dirItem = new QTableWidgetItem(direction);
    if (direction == tr("发布")) {
        dirItem->setForeground(QBrush(QColor("#4CAF50"))); // 绿色
    } else if (direction == tr("接收")) {
        dirItem->setForeground(QBrush(QColor("#2196F3"))); // 蓝色
    } else {
        dirItem->setForeground(QBrush(Qt::red)); // 失败用红色
    }
    m_recentMessages->setItem(0, 1, dirItem);

    // 主题
    m_recentMessages->setItem(0, 2, new QTableWidgetItem(topic));

    // 内容（截断过长内容）
    QString displayPayload = payload.length() > 80
        ? payload.left(80) + "..." : payload;
    m_recentMessages->setItem(0, 3, new QTableWidgetItem(displayPayload));

    // QoS
    m_recentMessages->setItem(0, 4, new QTableWidgetItem(QString::number(qos)));

    // 如果超过最大行数，裁剪末尾的旧行
    trimMessageTable();
}

void TabMessageMonitor::trimMessageTable()
{
    // 保持表格行数不超过最大值
    while (m_recentMessages->rowCount() > kMaxMessageRows) {
        m_recentMessages->removeRow(m_recentMessages->rowCount() - 1);
    }
}

void TabMessageMonitor::addTopicToTree(const QString &topic)
{
    // 将主题按 '/' 分层，构建树状结构文本
    QStringList levels = topic.split('/');

    // 构建缩进字符串
    QString treeLine;
    for (int i = 0; i < levels.size(); ++i) {
        if (i == 0) {
            treeLine += levels[i];
        } else {
            treeLine += QString("  ").repeated(i) + "├─ " + levels[i];
        }
        if (i < levels.size() - 1) {
            treeLine += "\n";
        }
    }

    // 追加到主题树显示区（避免重复添加相同主题）
    QString currentText = m_topicTree->toPlainText();
    if (!currentText.contains(topic)) {
        if (!currentText.isEmpty()) {
            currentText += "\n";
        }
        currentText += treeLine;
        m_topicTree->setPlainText(currentText);

        // 滚动到底部
        QTextCursor cursor = m_topicTree->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_topicTree->setTextCursor(cursor);
    }
}
