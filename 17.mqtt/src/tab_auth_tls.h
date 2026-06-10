#ifndef TAB_AUTH_TLS_H
#define TAB_AUTH_TLS_H

#include <QWidget>

class MqttClientWrapper;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QPushButton;
class QLabel;
class QTextEdit;

/**
 * @brief Tab 6: 认证与 TLS/SSL
 *
 * 用于配置 MQTT 连接的用户名/密码认证和 TLS/SSL 加密。
 * 注意：认证和 TLS 配置必须在连接到 Broker 之前完成。
 */
class TabAuthTls : public QWidget
{
    Q_OBJECT

public:
    explicit TabAuthTls(MqttClientWrapper *wrapper, QWidget *parent = nullptr);

private slots:
    void onBrowseCaCert();
    void onBrowseClientCert();
    void onBrowsePrivateKey();
    void onTestConnection();
    void onConnectionStateChanged(bool connected);

private:
    void setupUi();
    void setupConnections();

    MqttClientWrapper *m_wrapper;

    // 认证区域
    QLineEdit    *m_usernameEdit;   // 用户名
    QLineEdit    *m_passwordEdit;    // 密码

    // TLS 配置区域
    QCheckBox     *m_enableTlsCheckBox;  // 启用 TLS
    QComboBox     *m_tlsVersionCombo;    // TLS 版本选择
    QPushButton   *m_browseCaCertBtn;     // 浏览 CA 证书按钮
    QPushButton   *m_browseClientCertBtn; // 浏览客户端证书按钮
    QPushButton   *m_browsePrivateKeyBtn; // 浏览私钥文件按钮
    QLabel        *m_caPathLabel;        // CA 证书路径显示
    QLabel        *m_certPathLabel;      // 客户端证书路径显示
    QLabel        *m_keyPathLabel;       // 私钥文件路径显示

    // 操作区域
    QPushButton   *m_testConnBtn;    // 测试连接按钮

    // 结果日志
    QTextEdit     *m_resultLog;      // 连接结果日志

    // 内部状态
    QString m_caCertPath;     // CA 证书文件路径
    QString m_clientCertPath; // 客户端证书文件路径
    QString m_privateKeyPath; // 私钥文件路径
};

#endif // TAB_AUTH_TLS_H
