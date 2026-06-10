#include "tab_auth_tls.h"
#include "mqtt_client_wrapper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QDateTime>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QFile>
#include <QGroupBox>

TabAuthTls::TabAuthTls(MqttClientWrapper *wrapper, QWidget *parent)
    : QWidget(parent)
    , m_wrapper(wrapper)
    , m_usernameEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_enableTlsCheckBox(new QCheckBox(tr("启用 TLS/SSL"), this))
    , m_tlsVersionCombo(new QComboBox(this))
    , m_browseCaCertBtn(new QPushButton(tr("浏览..."), this))
    , m_browseClientCertBtn(new QPushButton(tr("浏览..."), this))
    , m_browsePrivateKeyBtn(new QPushButton(tr("浏览..."), this))
    , m_caPathLabel(new QLabel(tr("(未选择)"), this))
    , m_certPathLabel(new QLabel(tr("(未选择)"), this))
    , m_keyPathLabel(new QLabel(tr("(未选择)"), this))
    , m_testConnBtn(new QPushButton(tr("测试连接"), this))
    , m_resultLog(new QTextEdit(this))
{
    setupUi();
    setupConnections();
}

void TabAuthTls::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== 重要提示 =====
    QLabel *warnLabel = new QLabel(
        tr("重要提示：认证和 TLS 配置必须在连接到 Broker 之前完成！\n"
           "连接建立后修改这些配置不会生效，需要先断开再重新连接。"),
        this);
    warnLabel->setWordWrap(true);
    warnLabel->setStyleSheet(
        "QLabel { color: red; font-weight: bold; padding: 8px; "
        "background-color: #fff3cd; border: 1px solid #ffc107; border-radius: 4px; }");
    mainLayout->addWidget(warnLabel);

    // ===== 认证配置组 =====
    QGroupBox *authGroup = new QGroupBox(tr("用户名/密码认证"), this);
    QGridLayout *authLayout = new QGridLayout(authGroup);

    authLayout->addWidget(new QLabel(tr("用户名:"), this), 0, 0);
    m_usernameEdit->setPlaceholderText(tr("输入 MQTT 用户名"));
    authLayout->addWidget(m_usernameEdit, 0, 1);

    authLayout->addWidget(new QLabel(tr("密码:"), this), 1, 0);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText(tr("输入 MQTT 密码"));
    authLayout->addWidget(m_passwordEdit, 1, 1);

    mainLayout->addWidget(authGroup);

    // ===== TLS/SSL 配置组 =====
    QGroupBox *tlsGroup = new QGroupBox(tr("TLS/SSL 加密"), this);
    QGridLayout *tlsLayout = new QGridLayout(tlsGroup);

    // 启用 TLS 复选框
    tlsLayout->addWidget(m_enableTlsCheckBox, 0, 0, 1, 3);

    // TLS 版本选择
    tlsLayout->addWidget(new QLabel(tr("TLS 版本:"), this), 1, 0);
    m_tlsVersionCombo->addItem(tr("TLSv1.0"), static_cast<int>(QSsl::TlsV1_0));
    m_tlsVersionCombo->addItem(tr("TLSv1.1"), static_cast<int>(QSsl::TlsV1_1));
    m_tlsVersionCombo->addItem(tr("TLSv1.2"), static_cast<int>(QSsl::TlsV1_2));
    // Qt5.13 无独立 TlsV1_3 枚举，使用 TlsV1_2 作为最高安全协议（底层 OpenSSL 会自动协商 TLSv1.3）
    m_tlsVersionCombo->addItem(tr("TLSv1.3 (自动协商)"), static_cast<int>(QSsl::TlsV1_2));
    m_tlsVersionCombo->setCurrentIndex(2); // 默认 TLSv1.2
    m_tlsVersionCombo->setEnabled(false);
    tlsLayout->addWidget(m_tlsVersionCombo, 1, 1, 1, 2);

    // CA 证书
    tlsLayout->addWidget(new QLabel(tr("CA 证书:"), this), 2, 0);
    tlsLayout->addWidget(m_caPathLabel, 2, 1);
    m_browseCaCertBtn->setEnabled(false);
    tlsLayout->addWidget(m_browseCaCertBtn, 2, 2);

    // 客户端证书
    tlsLayout->addWidget(new QLabel(tr("客户端证书:"), this), 3, 0);
    tlsLayout->addWidget(m_certPathLabel, 3, 1);
    m_browseClientCertBtn->setEnabled(false);
    tlsLayout->addWidget(m_browseClientCertBtn, 3, 2);

    // 私钥文件
    tlsLayout->addWidget(new QLabel(tr("私钥文件:"), this), 4, 0);
    tlsLayout->addWidget(m_keyPathLabel, 4, 1);
    m_browsePrivateKeyBtn->setEnabled(false);
    tlsLayout->addWidget(m_browsePrivateKeyBtn, 4, 2);

    mainLayout->addWidget(tlsGroup);

    // ===== 操作区域 =====
    QHBoxLayout *actionLayout = new QHBoxLayout();

    // 状态指示器（绿色锁图标表示 TLS 已启用，红色表示明文连接）
    QLabel *statusIcon = new QLabel(this);
    statusIcon->setText("🔓"); // 明文连接
    statusIcon->setStyleSheet("font-size: 20px;");
    actionLayout->addWidget(statusIcon);

    actionLayout->addStretch();
    m_testConnBtn->setStyleSheet(
        "QPushButton { background-color: #28a745; color: white; "
        "font-weight: bold; padding: 8px 16px; border-radius: 4px; }");
    actionLayout->addWidget(m_testConnBtn);

    mainLayout->addLayout(actionLayout);

    // ===== 结果日志 =====
    QGroupBox *logGroup = new QGroupBox(tr("连接日志"), this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);

    m_resultLog->setReadOnly(true);
    m_resultLog->setPlaceholderText(tr("连接尝试结果将显示在此处..."));
    logLayout->addWidget(m_resultLog);

    mainLayout->addWidget(logGroup);

    mainLayout->addStretch();

    // ===== TLS 启用/禁用联动 =====
    connect(m_enableTlsCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        m_tlsVersionCombo->setEnabled(checked);
        m_browseCaCertBtn->setEnabled(checked);
        m_browseClientCertBtn->setEnabled(checked);
        m_browsePrivateKeyBtn->setEnabled(checked);
    });
}

void TabAuthTls::setupConnections()
{
    // 证书文件浏览按钮
    connect(m_browseCaCertBtn, &QPushButton::clicked,
            this, &TabAuthTls::onBrowseCaCert);
    connect(m_browseClientCertBtn, &QPushButton::clicked,
            this, &TabAuthTls::onBrowseClientCert);
    connect(m_browsePrivateKeyBtn, &QPushButton::clicked,
            this, &TabAuthTls::onBrowsePrivateKey);

    // 测试连接按钮
    connect(m_testConnBtn, &QPushButton::clicked,
            this, &TabAuthTls::onTestConnection);

    // 监听连接状态变化
    connect(m_wrapper, &MqttClientWrapper::connectionStateChanged,
            this, &TabAuthTls::onConnectionStateChanged);
}

void TabAuthTls::onBrowseCaCert()
{
    // 浏览并选择 CA 证书文件
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("选择 CA 证书文件"), QString(),
        tr("证书文件 (*.pem *.crt *.cer *.der);;所有文件 (*)"));

    if (!filePath.isEmpty()) {
        m_caCertPath = filePath;
        m_caPathLabel->setText(filePath);
        m_caPathLabel->setToolTip(filePath);

        m_resultLog->append(
            QString("[%1] CA 证书已选择: %2")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(filePath));
    }
}

void TabAuthTls::onBrowseClientCert()
{
    // 浏览并选择客户端证书文件
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("选择客户端证书文件"), QString(),
        tr("证书文件 (*.pem *.crt *.cer *.der);;所有文件 (*)"));

    if (!filePath.isEmpty()) {
        m_clientCertPath = filePath;
        m_certPathLabel->setText(filePath);
        m_certPathLabel->setToolTip(filePath);

        m_resultLog->append(
            QString("[%1] 客户端证书已选择: %2")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(filePath));
    }
}

void TabAuthTls::onBrowsePrivateKey()
{
    // 浏览并选择私钥文件
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("选择私钥文件"), QString(),
        tr("私钥文件 (*.pem *.key *.der);;所有文件 (*)"));

    if (!filePath.isEmpty()) {
        m_privateKeyPath = filePath;
        m_keyPathLabel->setText(filePath);
        m_keyPathLabel->setToolTip(filePath);

        m_resultLog->append(
            QString("[%1] 私钥文件已选择: %2")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(filePath));
    }
}

void TabAuthTls::onTestConnection()
{
    m_resultLog->append(
        QString("[%1] ========== 开始测试连接 ==========")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));

    // 检查是否已连接
    if (m_wrapper->isConnected()) {
        m_resultLog->append(
            QString("[%1] <span style=\"color:orange;\">警告：当前已连接，将先断开再重新连接</span>")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        m_wrapper->disconnectFromBroker();
    }

    // 1. 应用认证配置（用户名和密码）
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (!username.isEmpty()) {
        m_wrapper->setUsername(username);
        m_wrapper->setPassword(password);
        m_resultLog->append(
            QString("[%1] 认证信息已配置 - 用户名: %2")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(username));
    } else {
        m_resultLog->append(
            QString("[%1] 未配置用户名，将使用匿名连接")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    }

    // 2. 应用 TLS 配置
    if (m_enableTlsCheckBox->isChecked()) {
        m_wrapper->setSslEnabled(true);

        // 设置 TLS 协议版本
        int protocolIndex = m_tlsVersionCombo->currentIndex();
        int protocolValue = m_tlsVersionCombo->itemData(protocolIndex).toInt();
        m_wrapper->setSslProtocol(static_cast<QSsl::SslProtocol>(protocolValue));

        m_resultLog->append(
            QString("[%1] TLS/SSL 已启用, 协议版本: %2")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(m_tlsVersionCombo->currentText()));

        // 加载 CA 证书
        if (!m_caCertPath.isEmpty()) {
            QList<QSslCertificate> caCerts =
                QSslCertificate::fromPath(m_caCertPath);
            if (!caCerts.isEmpty()) {
                m_wrapper->setCaCertificates(caCerts);
                m_resultLog->append(
                    QString("[%1] 已加载 %2 个 CA 证书")
                        .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                        .arg(caCerts.size()));
            } else {
                m_resultLog->append(
                    QString("[%1] <span style=\"color:red;\">警告：CA 证书加载失败</span>")
                        .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
            }
        }

        // 加载客户端证书
        if (!m_clientCertPath.isEmpty()) {
            QFile certFile(m_clientCertPath);
            if (certFile.open(QIODevice::ReadOnly)) {
                QSslCertificate cert(&certFile, QSsl::Pem);
                if (!cert.isNull()) {
                    m_wrapper->setLocalCertificate(cert);
                    m_resultLog->append(
                        QString("[%1] 客户端证书已加载")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
                } else {
                    m_resultLog->append(
                        QString("[%1] <span style=\"color:red;\">警告：客户端证书解析失败</span>")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
                }
                certFile.close();
            }
        }

        // 加载私钥
        if (!m_privateKeyPath.isEmpty()) {
            QFile keyFile(m_privateKeyPath);
            if (keyFile.open(QIODevice::ReadOnly)) {
                QSslKey key(&keyFile, QSsl::Rsa, QSsl::Pem);
                if (key.isNull()) {
                    // 尝试 EC 密钥
                    keyFile.seek(0);
                    key = QSslKey(&keyFile, QSsl::Ec, QSsl::Pem);
                }
                if (!key.isNull()) {
                    m_wrapper->setPrivateKey(key);
                    m_resultLog->append(
                        QString("[%1] 私钥已加载")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
                } else {
                    m_resultLog->append(
                        QString("[%1] <span style=\"color:red;\">警告：私钥解析失败（尝试了 RSA 和 EC 格式）</span>")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
                }
                keyFile.close();
            }
        }
    } else {
        m_wrapper->setSslEnabled(false);
        m_resultLog->append(
            QString("[%1] TLS/SSL 未启用，使用明文连接")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    }

    // 3. 发起连接
    m_resultLog->append(
        QString("[%1] 正在连接到 Broker...")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    m_wrapper->connectToBroker();
}

void TabAuthTls::onConnectionStateChanged(bool connected)
{
    QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss");

    if (connected) {
        // 连接成功
        bool tlsEnabled = m_enableTlsCheckBox->isChecked();

        if (tlsEnabled) {
            m_resultLog->append(
                QString("[%1] <span style=\"color:green; font-weight:bold;\">"
                        "✅ 连接成功（TLS/SSL 加密已启用 🔒）</span>")
                    .arg(timeStr));
        } else {
            m_resultLog->append(
                QString("[%1] <span style=\"color:green; font-weight:bold;\">"
                        "✅ 连接成功（明文连接 🔓）</span>")
                    .arg(timeStr));
        }

        m_resultLog->append(
            QString("[%1] ========== 连接测试完成 ==========")
                .arg(timeStr));
    } else {
        // 连接断开
        m_resultLog->append(
            QString("[%1] <span style=\"color:red;\">❌ 连接已断开</span>")
                .arg(timeStr));
    }
}
