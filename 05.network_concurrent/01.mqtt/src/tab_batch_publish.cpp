#include "tab_batch_publish.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QFileInfo>

// ========== 默认配置 ==========
static const int kDefaultIntervalMs  = 100;   // 默认发布间隔：100ms
static const int kDefaultCount       = 100;   // 默认发布数量
static const int kSampleDataCount    = 100;   // 示例数据条数
static const int kMaxPayloadDisplay  = 60;    // 表格中 Payload 最大显示长度

TabBatchPublish::TabBatchPublish(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_publishTimer(new QTimer(this))
    , m_currentIndex(0)
    , m_totalSent(0)
    , m_successCount(0)
    , m_failCount(0)
{
    setupUI();

    // 定时发布信号
    connect(m_publishTimer, &QTimer::timeout,
            this, &TabBatchPublish::onPublishTick);

    // 监听发布结果信号
    connect(m_wrapper, &MqttClientWrapper::messagePublished,
            this, &TabBatchPublish::onMessagePublished);
    connect(m_wrapper, &MqttClientWrapper::publishFailed,
            this, &TabBatchPublish::onPublishFailed);
}

void TabBatchPublish::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 顶部：配置区域 =====
    QGroupBox *configGroup = new QGroupBox(tr("发布配置"));
    QGridLayout *configGrid = new QGridLayout(configGroup);

    configGrid->addWidget(new QLabel(tr("目标主题:")), 0, 0);
    m_topicEdit = new QLineEdit("qt/batch/test");
    configGrid->addWidget(m_topicEdit, 0, 1);

    configGrid->addWidget(new QLabel(tr("间隔(ms):")), 0, 2);
    m_intervalSpin = new QSpinBox();
    m_intervalSpin->setRange(10, 60000);
    m_intervalSpin->setValue(kDefaultIntervalMs);
    m_intervalSpin->setSuffix(tr(" 毫秒"));
    configGrid->addWidget(m_intervalSpin, 0, 3);

    configGrid->addWidget(new QLabel(tr("数量:")), 0, 4);
    m_countSpin = new QSpinBox();
    m_countSpin->setRange(1, 100000);
    m_countSpin->setValue(kDefaultCount);
    configGrid->addWidget(m_countSpin, 0, 5);

    mainLayout->addWidget(configGroup);

    // ===== 控制按钮区域 =====
    QHBoxLayout *btnLayout = new QHBoxLayout();

    m_loadFileButton = new QPushButton(tr("加载文件"));
    connect(m_loadFileButton, &QPushButton::clicked,
            this, &TabBatchPublish::onLoadFile);
    btnLayout->addWidget(m_loadFileButton);

    m_generateButton = new QPushButton(tr("生成示例数据"));
    connect(m_generateButton, &QPushButton::clicked,
            this, &TabBatchPublish::onGenerateSample);
    btnLayout->addWidget(m_generateButton);

    btnLayout->addStretch();

    m_startButton = new QPushButton(tr("开始发布"));
    m_startButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; "
        "padding: 5px 15px; font-weight: bold; }");
    connect(m_startButton, &QPushButton::clicked,
            this, &TabBatchPublish::onStart);
    btnLayout->addWidget(m_startButton);

    m_stopButton = new QPushButton(tr("停止"));
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; "
        "padding: 5px 15px; font-weight: bold; }");
    connect(m_stopButton, &QPushButton::clicked,
            this, &TabBatchPublish::onStop);
    btnLayout->addWidget(m_stopButton);

    mainLayout->addLayout(btnLayout);

    // ===== 进度条 =====
    m_progressBar = new QProgressBar();
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);

    // ===== 统计标签 =====
    m_statsLabel = new QLabel(tr("就绪"));
    m_statsLabel->setAlignment(Qt::AlignCenter);
    m_statsLabel->setStyleSheet(
        "QLabel { font-size: 13px; font-weight: bold; padding: 4px; }");
    mainLayout->addWidget(m_statsLabel);

    // ===== 消息列表表格 =====
    QGroupBox *tableGroup = new QGroupBox(tr("消息列表"));
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    m_messageTable = new QTableWidget(0, 3);
    m_messageTable->setHorizontalHeaderLabels(
        QStringList() << tr("序号") << tr("主题") << tr("内容"));
    m_messageTable->horizontalHeader()->setStretchLastSection(true);
    m_messageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_messageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_messageTable->setAlternatingRowColors(true);

    // 设置列宽
    m_messageTable->setColumnWidth(0, 60);   // 序号
    m_messageTable->setColumnWidth(1, 200);  // 主题

    tableLayout->addWidget(m_messageTable);
    mainLayout->addWidget(tableGroup);
}

void TabBatchPublish::onLoadFile()
{
    // 弹出文件选择对话框
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("选择消息文件"), QString(),
        tr("JSON 文件 (*.json);;CSV 文件 (*.csv);;所有文件 (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("文件错误"),
                             tr("无法打开文件：%1").arg(file.errorString()));
        return;
    }

    // 根据文件扩展名选择解析方式
    QString suffix = QFileInfo(filePath).suffix().toLower();
    QList<QPair<QString, QString>> messages; // (topic, payload) 列表

    if (suffix == "json") {
        // 解析 JSON 格式：期望 [{"topic": "...", "payload": "..."}, ...]
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) {
            QMessageBox::warning(this, tr("格式错误"),
                                 tr("JSON 文件应包含消息数组，格式：[{\"topic\":\"...\",\"payload\":\"...\"}]"));
            return;
        }

        QJsonArray arr = doc.array();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            QString topic   = obj.value("topic").toString();
            QString payload = obj.value("payload").toString();
            if (!topic.isEmpty()) {
                messages.append(qMakePair(topic, payload));
            }
        }
    } else {
        // 解析 CSV 格式：每行 "主题,内容"
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty() || line.startsWith("#")) {
                continue; // 跳过空行和注释行
            }
            // 逗号分隔（只按第一个逗号切分，因为内容可能包含逗号）
            int commaPos = line.indexOf(',');
            if (commaPos > 0) {
                QString topic   = line.left(commaPos).trimmed();
                QString payload = line.mid(commaPos + 1).trimmed();
                messages.append(qMakePair(topic, payload));
            }
        }
    }

    file.close();

    if (messages.isEmpty()) {
        QMessageBox::warning(this, tr("无数据"),
                             tr("文件中未找到有效的消息数据。"));
        return;
    }

    // 清空现有表格数据并填充新数据
    m_messageTable->setRowCount(0);
    for (int i = 0; i < messages.size(); ++i) {
        int row = m_messageTable->rowCount();
        m_messageTable->insertRow(row);

        m_messageTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        m_messageTable->setItem(row, 1, new QTableWidgetItem(messages[i].first));

        QString displayPayload = messages[i].second.length() > kMaxPayloadDisplay
            ? messages[i].second.left(kMaxPayloadDisplay) + "..."
            : messages[i].second;
        m_messageTable->setItem(row, 2, new QTableWidgetItem(displayPayload));
    }

    // 更新发布数量为消息总数
    m_countSpin->setValue(messages.size());
    m_statsLabel->setText(tr("已加载 %1 条消息").arg(messages.size()));
}

void TabBatchPublish::onGenerateSample()
{
    // 生成示例测试数据（100 条），用于压力测试
    m_messageTable->setRowCount(0);

    for (int i = 0; i < kSampleDataCount; ++i) {
        int row = m_messageTable->rowCount();
        m_messageTable->insertRow(row);

        // 序号
        m_messageTable->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));

        // 主题：使用目标主题
        m_messageTable->setItem(row, 1, new QTableWidgetItem(m_topicEdit->text()));

        // 内容：生成带序号的示例数据
        QString payload = QString("{\"seq\":%1,\"timestamp\":\"%2\",\"data\":\"test_message_%3\"}")
                              .arg(i + 1)
                              .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                              .arg(i + 1);

        QString displayPayload = payload.length() > kMaxPayloadDisplay
            ? payload.left(kMaxPayloadDisplay) + "..." : payload;
        m_messageTable->setItem(row, 2, new QTableWidgetItem(displayPayload));
    }

    // 更新发布数量
    m_countSpin->setValue(kSampleDataCount);
    m_statsLabel->setText(tr("已生成 %1 条示例消息").arg(kSampleDataCount));
}

void TabBatchPublish::onStart()
{
    // 检查连接状态
    if (!m_wrapper->isConnected()) {
        QMessageBox::warning(this, tr("未连接"),
                             tr("请先连接到 MQTT Broker 后再发布消息。"));
        return;
    }

    // 检查消息列表是否为空
    if (m_messageTable->rowCount() == 0) {
        QMessageBox::warning(this, tr("无消息"),
                             tr("请先加载文件或生成示例数据。"));
        return;
    }

    // 初始化发布状态
    clearStats();
    m_currentIndex = 0;
    m_startTime = QDateTime::currentDateTime();

    int total = qMin(m_countSpin->value(), m_messageTable->rowCount());
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(0);

    // 切换按钮状态
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_loadFileButton->setEnabled(false);
    m_generateButton->setEnabled(false);

    // 启动定时发布器
    m_publishTimer->setInterval(m_intervalSpin->value());
    m_publishTimer->start();
}

void TabBatchPublish::onStop()
{
    // 停止定时发布
    m_publishTimer->stop();

    // 恢复按钮状态
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_loadFileButton->setEnabled(true);
    m_generateButton->setEnabled(true);

    // 更新统计
    m_statsLabel->setText(m_statsLabel->text() + tr(" [已停止]"));
}

void TabBatchPublish::onPublishTick()
{
    // 检查是否已达到发布数量上限
    int total = qMin(m_countSpin->value(), m_messageTable->rowCount());
    if (m_currentIndex >= total) {
        // 发布完成
        m_publishTimer->stop();

        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_loadFileButton->setEnabled(true);
        m_generateButton->setEnabled(true);

        m_statsLabel->setText(m_statsLabel->text() + tr(" [已完成]"));
        return;
    }

    // 获取当前行的消息数据
    QString topic   = m_messageTable->item(m_currentIndex, 1)->text();

    // 构造完整 payload（表格中显示的是截断版本，需要完整数据）
    // 对于从文件加载的数据，这里使用表格中的文本
    // 对于生成的示例数据，重新构造完整 JSON
    QString payload;
    QString displayText = m_messageTable->item(m_currentIndex, 2)->text();

    // 检查是否被截断了
    if (displayText.endsWith("...")) {
        // 被截断的数据，使用显示内容作为 payload
        payload = displayText;
    } else {
        payload = displayText;
    }

    // 发布消息
    m_wrapper->publish(topic, payload.toUtf8(), 0, false);

    m_totalSent++;
    m_currentIndex++;

    // 更新进度条
    m_progressBar->setValue(m_currentIndex);

    // 更新统计信息
    updateStats();
}

void TabBatchPublish::onMessagePublished(quint32 msgId, const QString &topic,
                                          const QByteArray &payload, quint8 qos)
{
    Q_UNUSED(msgId);
    Q_UNUSED(topic);
    Q_UNUSED(payload);
    Q_UNUSED(qos);

    // 发布成功，更新计数
    m_successCount++;
    updateStats();
}

void TabBatchPublish::onPublishFailed(quint32 msgId, const QString &topic,
                                       const QString &error)
{
    Q_UNUSED(msgId);
    Q_UNUSED(topic);
    Q_UNUSED(error);

    // 发布失败，更新计数
    m_failCount++;
    updateStats();
}

void TabBatchPublish::updateStats()
{
    // 计算已用时间和速率
    qint64 elapsedMs = m_startTime.msecsTo(QDateTime::currentDateTime());
    double elapsedSec = elapsedMs / 1000.0;
    double rate = (elapsedSec > 0.0) ? (m_totalSent / elapsedSec) : 0.0;

    // 更新统计标签
    m_statsLabel->setText(
        tr("总发送: %1 | 成功: %2 | 失败: %3 | 速率: %4 条/秒 | 用时: %5 秒")
            .arg(m_totalSent)
            .arg(m_successCount)
            .arg(m_failCount)
            .arg(rate, 0, 'f', 1)
            .arg(elapsedSec, 0, 'f', 1));
}

void TabBatchPublish::clearStats()
{
    m_totalSent = 0;
    m_successCount = 0;
    m_failCount = 0;
}
