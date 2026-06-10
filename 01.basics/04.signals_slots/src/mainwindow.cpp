#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "counter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_counter(new Counter(this))
    , m_connected(false)
{
    m_ui->setupUi(this);

    // Build UI in code using the central layout from the .ui file
    auto *centralLayout = m_ui->centralLayout;

    // Count display
    m_countLabel = new QLabel("Count: 0", this);
    m_countLabel->setAlignment(Qt::AlignCenter);
    m_countLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px;");
    centralLayout->addWidget(m_countLabel);

    // Counter buttons
    auto *counterLayout = new QHBoxLayout();
    m_incrementButton = new QPushButton("Increment", this);
    m_decrementButton = new QPushButton("Decrement", this);
    m_resetButton = new QPushButton("Reset", this);
    counterLayout->addWidget(m_incrementButton);
    counterLayout->addWidget(m_decrementButton);
    counterLayout->addWidget(m_resetButton);
    centralLayout->addLayout(counterLayout);

    // Connect / Disconnect buttons
    auto *connLayout = new QHBoxLayout();
    m_connectButton = new QPushButton("Connect", this);
    m_disconnectButton = new QPushButton("Disconnect", this);
    m_disconnectButton->setEnabled(false);
    connLayout->addWidget(m_connectButton);
    connLayout->addWidget(m_disconnectButton);
    centralLayout->addLayout(connLayout);

    // Status label
    m_statusLabel = new QLabel("Signal is disconnected. Click Connect first.", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setFrameShape(QFrame::StyledPanel);
    m_statusLabel->setWordWrap(true);
    centralLayout->addWidget(m_statusLabel);

    centralLayout->addStretch();

    // Connect counter action buttons to counter methods
    // Demonstrate: connect with functor (lambda)
    connect(m_incrementButton, &QPushButton::clicked, this, [this]() {
        m_counter->increment();
    });

    connect(m_decrementButton, &QPushButton::clicked, this, [this]() {
        m_counter->decrement();
    });

    // Traditional connect with member function slot
    connect(m_resetButton, &QPushButton::clicked,
            this, [this]() {
        m_counter->reset();
    });

    // Connect / Disconnect buttons
    connect(m_connectButton, &QPushButton::clicked,
            this, &MainWindow::doConnect);
    connect(m_disconnectButton, &QPushButton::clicked,
            this, &MainWindow::doDisconnect);

    // Signal-to-signal connection: relay countChanged to a signal we can observe
    // (demonstrate that signals can be connected to other signals)
    connect(m_counter, &Counter::countChanged,
            m_counter, &Counter::countChanged);

    // Start connected
    doConnect();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::doConnect()
{
    if (m_connected) {
        return;
    }

    // Demonstrate: connect with custom signal carrying int parameter
    connect(m_counter, &Counter::countChanged,
            this, &MainWindow::onCountChanged);

    m_connected = true;
    updateConnectionState();
    onCountChanged(m_counter->value());
}

void MainWindow::doDisconnect()
{
    if (!m_connected) {
        return;
    }

    disconnect(m_counter, &Counter::countChanged,
               this, &MainWindow::onCountChanged);

    m_connected = false;
    updateConnectionState();
}

void MainWindow::onCountChanged(int value)
{
    m_statusLabel->setText(
        QString("Signal received! Counter value changed to: %1").arg(value));
    updateCountDisplay();
}

void MainWindow::updateConnectionState()
{
    if (m_connected) {
        m_statusLabel->setText("Signal connected. Counter will update display.");
        m_connectButton->setEnabled(false);
        m_disconnectButton->setEnabled(true);
    } else {
        m_statusLabel->setText("Signal disconnected. Counter changes won't update display.");
        m_connectButton->setEnabled(true);
        m_disconnectButton->setEnabled(false);
    }
}

void MainWindow::updateCountDisplay()
{
    m_countLabel->setText(QString("Count: %1").arg(m_counter->value()));
}
