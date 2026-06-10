#include "tab_topic_wildcard.h"
#include "mqtt_client_wrapper.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QMessageBox>
#include <QTextCursor>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>

// ============================================================
// 构造函数
// ============================================================

TabTopicWildcard::TabTopicWildcard(MqttClientWrapper *client, QWidget *parent)
    : QWidget(parent)
    , m_client(client)
    , m_editTopic(nullptr)
    , m_btnSubscribe(nullptr)
    , m_btnUnsubscribe(nullptr)
    , m_listSubscriptions(nullptr)
    , m_treeTopics(nullptr)
    , m_textMessages(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ============ 顶部：订阅输入 + 预设按钮 ============
    QGroupBox *groupSub = new QGroupBox("订阅通配符主题");
    QVBoxLayout *subLayout = new QVBoxLayout(groupSub);

    // 输入行
    QHBoxLayout *inputRow = new QHBoxLayout();
    inputRow->addWidget(new QLabel("主题过滤器:"));
    m_editTopic = new QLineEdit();
    m_editTopic->setPlaceholderText("输入带通配符的主题，例如: sensor/+/temp");
    inputRow->addWidget(m_editTopic, 1);

    m_btnSubscribe = new QPushButton("订阅");
    m_btnSubscribe->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 6px 16px; }");
    inputRow->addWidget(m_btnSubscribe);

    m_btnUnsubscribe = new QPushButton("取消订阅");
    m_btnUnsubscribe->setEnabled(false);
    inputRow->addWidget(m_btnUnsubscribe);

    subLayout->addLayout(inputRow);

    // ---- 预设通配符按钮 ----
    // 提供常用通配符主题，方便快速测试
    // MQTT 通配符规则：
    //   "+" 匹配单层主题（例如 "sensor/+/temp" 匹配 "sensor/room1/temp"）
    //   "#" 匹配多层主题（例如 "sensor/#" 匹配 "sensor/room1/temp/avg"）
    QHBoxLayout *presetRow = new QHBoxLayout();
    presetRow->addWidget(new QLabel("预设:"));

    QStringList presets;
    presets << "sensor/+/temp" << "sensor/#" << "device/+/status" << "home/+/+/power";

    foreach (const QString &preset, presets) {
        QPushButton *btn = new QPushButton(preset);
        btn->setToolTip(QString("订阅 %1").arg(preset));
        btn->setStyleSheet("QPushButton { padding: 4px 10px; }");
        presetRow->addWidget(btn);
        m_presetButtons.append(btn);
    }

    presetRow->addStretch();
    subLayout->addLayout(presetRow);

    mainLayout->addWidget(groupSub);

    // ============ 中部：左侧订阅列表 + 右侧主题树 ============
    QHBoxLayout *middleLayout = new QHBoxLayout();

    // ---- 左侧：活跃订阅列表 ----
    QGroupBox *groupList = new QGroupBox("活跃订阅");
    QVBoxLayout *listLayout = new QVBoxLayout(groupList);

    m_listSubscriptions = new QListWidget();
    m_listSubscriptions->setMaximumWidth(220);
    listLayout->addWidget(m_listSubscriptions);

    middleLayout->addWidget(groupList);

    // ---- 右侧：主题树 + 消息区 ----
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // 主题树：将收到的消息主题解析为层级结构
    // 例如 "sensor/room1/temp" 会显示为 sensor → room1 → temp
    QGroupBox *groupTree = new QGroupBox("主题层级树（自动构建）");
    QVBoxLayout *treeLayout = new QVBoxLayout(groupTree);

    m_treeTopics = new QTreeWidget();
    m_treeTopics->setHeaderLabels(QStringList() << "层级");
    m_treeTopics->setMaximumHeight(200);
    treeLayout->addWidget(m_treeTopics);

    rightLayout->addWidget(groupTree);

    // 消息接收区：显示收到的消息并高亮匹配的订阅
    QGroupBox *groupMsg = new QGroupBox("接收到的消息（匹配高亮）");
    QVBoxLayout *msgLayout = new QVBoxLayout(groupMsg);

    m_textMessages = new QPlainTextEdit();
    m_textMessages->setReadOnly(true);
    m_textMessages->setFont(QFont("Consolas", 9));
    msgLayout->addWidget(m_textMessages);

    rightLayout->addWidget(groupMsg, 1);

    middleLayout->addLayout(rightLayout, 1);

    mainLayout->addLayout(middleLayout);

    // ============ 信号连接 ============
    connect(m_btnSubscribe, &QPushButton::clicked, this, &TabTopicWildcard::onSubscribeWildcard);
    connect(m_btnUnsubscribe, &QPushButton::clicked, this, &TabTopicWildcard::onUnsubscribeWildcard);

    // 所有预设按钮共享同一个槽函数，通过 sender() 区分
    foreach (QPushButton *btn, m_presetButtons) {
        connect(btn, &QPushButton::clicked, this, &TabTopicWildcard::onPresetClicked);
    }

    // 监听所有收到的消息
    connect(m_client, &MqttClientWrapper::messageReceived,
            this, &TabTopicWildcard::onMessageReceived);
}

// ============================================================
// 订阅通配符主题
// ============================================================

void TabTopicWildcard::onSubscribeWildcard()
{
    QString topic = m_editTopic->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入主题过滤器！");
        return;
    }

    // 检查重复
    for (int i = 0; i < m_listSubscriptions->count(); ++i) {
        if (m_listSubscriptions->item(i)->text() == topic) {
            QMessageBox::information(this, "提示", "该主题已在订阅列表中。");
            return;
        }
    }

    // 通配符订阅的 QoS 通常设为最高级别，以确保不丢失匹配的消息
    QMqttSubscription *sub = m_client->subscribe(topic, 2);
    if (sub) {
        QListWidgetItem *item = new QListWidgetItem(topic);
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<void *>(sub)));
        m_listSubscriptions->addItem(item);

        m_btnUnsubscribe->setEnabled(true);
    }
}

// ============================================================
// 取消订阅
// ============================================================

void TabTopicWildcard::onUnsubscribeWildcard()
{
    QListWidgetItem *item = m_listSubscriptions->currentItem();
    if (!item) {
        QMessageBox::information(this, "提示", "请先选择要取消的订阅。");
        return;
    }

    QString topic = item->text();
    m_client->unsubscribe(topic);
    delete m_listSubscriptions->takeItem(m_listSubscriptions->row(item));

    if (m_listSubscriptions->count() == 0) {
        m_btnUnsubscribe->setEnabled(false);
    }
}

// ============================================================
// 预设通配符按钮点击 —— 自动填入输入框并订阅
// ============================================================

void TabTopicWildcard::onPresetClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;

    m_editTopic->setText(btn->text());
    onSubscribeWildcard();
}

// ============================================================
// 收到消息 —— 更新主题树、高亮匹配的订阅、显示消息
// ============================================================

void TabTopicWildcard::onMessageReceived(const QString &topic,
                                          const QByteArray &payload,
                                          quint8 qos, bool retain)
{
    Q_UNUSED(qos);
    Q_UNUSED(retain);

    // 1. 将主题添加到层级树
    addToTopicTree(topic);

    // 2. 高亮匹配的订阅条目
    highlightMatchingSubs(topic);

    // 3. 在消息区显示
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    m_textMessages->appendPlainText(
        QString("[%1] %2 → %3").arg(timestamp, topic, QString::fromUtf8(payload)));
}

// ============================================================
// 将主题添加到层级树
//
// MQTT 主题使用 "/" 作为层级分隔符。
// 例如 "sensor/room1/temp" 被拆分为 sensor → room1 → temp 三级节点。
// 如果节点已存在则复用，否则创建新节点。
// ============================================================

void TabTopicWildcard::addToTopicTree(const QString &topic)
{
    QStringList levels = topic.split('/');

    QTreeWidgetItem *parent = m_treeTopics->invisibleRootItem();
    foreach (const QString &level, levels) {
        // 在当前父节点的子节点中查找是否已存在同名节点
        QTreeWidgetItem *found = nullptr;
        for (int i = 0; i < parent->childCount(); ++i) {
            if (parent->child(i)->text(0) == level) {
                found = parent->child(i);
                break;
            }
        }

        if (!found) {
            found = new QTreeWidgetItem(parent, QStringList() << level);
        }
        parent = found;
    }

    m_treeTopics->expandAll();
}

// ============================================================
// 高亮匹配的订阅条目
//
// 遍历所有活跃订阅，检查通配符过滤器是否匹配当前主题。
// MQTT 通配符规则：
//   "+" 匹配且仅匹配一个层级（不匹配空层级）
//   "#" 必须是主题过滤器的最后一层，匹配当前层级及之后所有层级
// ============================================================

void TabTopicWildcard::highlightMatchingSubs(const QString &topic)
{
    QStringList topicLevels = topic.split('/');

    for (int i = 0; i < m_listSubscriptions->count(); ++i) {
        QListWidgetItem *item = m_listSubscriptions->item(i);
        QString filter = item->text();
        QStringList filterLevels = filter.split('/');

        // 简单通配符匹配算法
        bool matched = false;

        if (filter.endsWith('#')) {
            // "#" 多级通配符：匹配从当前位置到末尾的所有内容
            // 例如 "sensor/#" 匹配 "sensor/room1/temp"
            QStringList prefix = filterLevels.mid(0, filterLevels.size() - 1);
            if (topicLevels.size() >= prefix.size()) {
                matched = true;
                for (int j = 0; j < prefix.size(); ++j) {
                    if (prefix[j] != "+" && prefix[j] != topicLevels[j]) {
                        matched = false;
                        break;
                    }
                }
            }
        } else if (filterLevels.size() == topicLevels.size()) {
            // 层级数量相同，逐层比较
            matched = true;
            for (int j = 0; j < filterLevels.size(); ++j) {
                if (filterLevels[j] != "+" && filterLevels[j] != topicLevels[j]) {
                    matched = false;
                    break;
                }
            }
        }

        // 匹配的订阅高亮显示（绿色背景），不匹配的恢复默认
        if (matched) {
            item->setBackground(Qt::green);
            item->setForeground(Qt::black);
        } else {
            item->setBackground(Qt::transparent);
            item->setForeground(Qt::black);
        }
    }
}
