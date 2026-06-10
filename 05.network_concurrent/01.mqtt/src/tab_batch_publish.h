#ifndef TAB_BATCH_PUBLISH_H
#define TAB_BATCH_PUBLISH_H

#include <QWidget>
#include <QDateTime>

class MqttClientWrapper;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QProgressBar;
class QLabel;
class QTableWidget;
class QTimer;

/**
 * @brief Tab 8: 批量发布
 *
 * 用于批量测试 MQTT 消息发布功能，支持：
 *   1. 从 JSON/CSV 文件加载消息列表
 *   2. 自动生成示例数据（用于压力测试）
 *   3. 按指定间隔和数量定时发布消息
 *   4. 实时显示发布进度和统计信息（成功/失败/速率）
 */
class TabBatchPublish : public QWidget
{
    Q_OBJECT

public:
    explicit TabBatchPublish(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onLoadFile();           // 加载消息文件
    void onGenerateSample();     // 生成示例数据
    void onStart();              // 开始批量发布
    void onStop();               // 停止批量发布
    void onPublishTick();        // 定时发布一条消息
    void onMessagePublished(quint32 msgId, const QString &topic,
                            const QByteArray &payload, quint8 qos); // 发布成功
    void onPublishFailed(quint32 msgId, const QString &topic,
                          const QString &error);                      // 发布失败

private:
    void setupUI();
    void updateStats();
    void clearStats();

    MqttClientWrapper *m_wrapper;

    // 顶部配置区域
    QLineEdit    *m_topicEdit;       // 目标主题输入框
    QSpinBox     *m_intervalSpin;    // 发布间隔（毫秒）
    QSpinBox     *m_countSpin;       // 发布数量

    // 控制按钮
    QPushButton  *m_loadFileButton;     // 加载文件按钮
    QPushButton  *m_generateButton;    // 生成示例数据按钮
    QPushButton  *m_startButton;       // 开始发布按钮
    QPushButton  *m_stopButton;         // 停止发布按钮

    // 进度与统计
    QProgressBar  *m_progressBar;   // 进度条
    QLabel        *m_statsLabel;    // 统计标签

    // 消息列表表格（列：序号, 主题, 内容）
    QTableWidget  *m_messageTable;

    // 定时发布器
    QTimer        *m_publishTimer;

    // 发布状态
    int m_currentIndex;         // 当前发布到的索引
    int m_totalSent;            // 总发送数
    int m_successCount;         // 成功数
    int m_failCount;            // 失败数
    QDateTime m_startTime;      // 开始时间（用于计算速率）
};

#endif // TAB_BATCH_PUBLISH_H
