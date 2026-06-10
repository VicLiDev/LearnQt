#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_label(new QLabel("Ready. Click the button!", this))
    , m_clickButton(new QPushButton("Click Me", this))
    , m_resetButton(new QPushButton("Reset", this))
    , m_counter(0)
{
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);

    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("font-size: 18px; font-weight: bold;");

    mainLayout->addWidget(m_label);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_clickButton);
    buttonLayout->addWidget(m_resetButton);
    mainLayout->addLayout(buttonLayout);

    mainLayout->setContentsMargins(20, 20, 20, 20);
    setCentralWidget(centralWidget);

    // Lambda-based connect (modern C++ style)
    connect(m_clickButton, &QPushButton::clicked, this, [this]() {
        ++m_counter;
        updateLabel();
    });

    // Traditional connect with member function slot
    connect(m_resetButton, &QPushButton::clicked,
            this, &MainWindow::onResetClicked);
}

MainWindow::~MainWindow() = default;

void MainWindow::onResetClicked()
{
    m_counter = 0;
    updateLabel();
}

void MainWindow::updateLabel()
{
    if (m_counter == 0) {
        m_label->setText("Ready. Click the button!");
    } else {
        m_label->setText(QString("Hello Qt! Clicked %1 times").arg(m_counter));
    }
}
