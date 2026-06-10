#ifndef TAB_TOPIC_WILDCARD_H
#define TAB_TOPIC_WILDCARD_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QListWidget;
class QTreeWidget;
class QPlainTextEdit;
class MqttClientWrapper;

/**
 * @brief Tab 3: 主题与通配符演示
 *
 * 演示 MQTT 主题层级结构和通配符机制：
 *   - "+"（单级通配符）：匹配主题中某一层的任意字符串
 *   - "#"（多级通配符）：匹配主题中从当前位置到末尾的所有层级
 *   - 主题树可视化：根据收到的消息自动构建主题层级树
 *   - 高亮匹配：收到消息时高亮显示哪些通配符订阅命中
 */
class TabTopicWildcard : public QWidget
{
    Q_OBJECT

public:
    explicit TabTopicWildcard(MqttClientWrapper *client, QWidget *parent = nullptr);

private slots:
    void onSubscribeWildcard();    // 订阅通配符主题
    void onUnsubscribeWildcard();   // 取消订阅
    void onPresetClicked();        // 预设通配符按钮点击（由 sender() 判断）
    void onMessageReceived(const QString &topic, const QByteArray &payload,
                            quint8 qos, bool retain);   // 收到消息

private:
    MqttClientWrapper *m_client;

    // 顶部：订阅输入
    QLineEdit   *m_editTopic;          // 自定义订阅主题输入框
    QPushButton *m_btnSubscribe;       // 订阅按钮
    QPushButton *m_btnUnsubscribe;    // 取消订阅按钮

    // 预设通配符按钮列表
    QList<QPushButton *> m_presetButtons;

    // 左侧：活跃订阅列表（显示通配符主题）
    QListWidget *m_listSubscriptions;

    // 右上：主题树（可视化主题层级结构）
    QTreeWidget *m_treeTopics;

    // 右下：消息接收区（带高亮）
    QPlainTextEdit *m_textMessages;

    // 主题树操作辅助
    void addToTopicTree(const QString &topic);   // 根据主题向树中添加节点
    void highlightMatchingSubs(const QString &topic);  // 高亮匹配的订阅
};

#endif // TAB_TOPIC_WILDCARD_H
