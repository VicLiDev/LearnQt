#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "mqtt_client_wrapper.h"

class TabPublishSubscribe;
class TabQosLevel;
class TabTopicWildcard;
class TabLwt;
class TabRetained;
class TabAuthTls;
class TabRequestResponse;
class TabBatchPublish;
class TabMessageMonitor;

/**
 * @brief MQTT 学习 Demo 主窗口
 *
 * 布局：左侧 = 连接配置面板，右侧 = QTabWidget（9 个场景标签页）
 * 所有 Tab 共享同一个 MqttClientWrapper 实例。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    MqttClientWrapper *mqttClient() const;

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onConnectionStateChanged(bool connected);
    void onLogMessage(const QString &msg, const QString &level);

private:
    void setupUI();
    void setupConnectionPanel(QWidget *parent);

    // 左侧连接面板控件
    QLineEdit      *m_editHost;
    QSpinBox       *m_spinPort;
    QLineEdit      *m_editClientId;
    QLineEdit      *m_editUsername;
    QLineEdit      *m_editPassword;
    QSpinBox       *m_spinKeepAlive;
    QCheckBox      *m_checkCleanSession;
    QPushButton     *m_btnConnect;
    QPushButton     *m_btnDisconnect;
    QLabel          *m_labelStatus;
    QTextEdit       *m_textLog;

    // MQTT 客户端
    MqttClientWrapper *m_mqttClient;

    // 9 个 Tab
    QTabWidget *m_tabWidget;
    TabPublishSubscribe  *m_tabPubSub;
    TabQosLevel          *m_tabQos;
    TabTopicWildcard     *m_tabWildcard;
    TabLwt               *m_tabLwt;
    TabRetained          *m_tabRetained;
    TabAuthTls           *m_tabAuthTls;
    TabRequestResponse   *m_tabReqResp;
    TabBatchPublish      *m_tabBatch;
    TabMessageMonitor    *m_tabMonitor;
};

#endif // MAINWINDOW_H
