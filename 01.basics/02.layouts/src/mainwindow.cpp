#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Tab 1: VBoxLayout - 3 buttons stacked vertically with spacing
    auto *vboxPage = new QWidget();
    auto *vboxLayout = new QVBoxLayout(vboxPage);
    vboxLayout->setSpacing(15);
    vboxLayout->addWidget(new QPushButton("Button 1 (top)", vboxPage));
    vboxLayout->addWidget(new QPushButton("Button 2 (middle)", vboxPage));
    vboxLayout->addWidget(new QPushButton("Button 3 (bottom)", vboxPage));
    vboxLayout->addStretch();
    m_ui->tabWidget->addTab(vboxPage, "VBoxLayout");

    // Tab 2: HBoxLayout - 4 buttons horizontally with stretch
    auto *hboxPage = new QWidget();
    auto *hboxLayout = new QHBoxLayout(hboxPage);
    hboxLayout->addWidget(new QPushButton("Btn 1", hboxPage), 1);
    hboxLayout->addWidget(new QPushButton("Btn 2", hboxPage), 2);
    hboxLayout->addWidget(new QPushButton("Btn 3", hboxPage), 1);
    hboxLayout->addWidget(new QPushButton("Btn 4", hboxPage), 3);
    m_ui->tabWidget->addTab(hboxPage, "HBoxLayout");

    // Tab 3: GridLayout - calculator-style layout
    auto *gridPage = new QWidget();
    auto *gridMainLayout = new QVBoxLayout(gridPage);

    auto *display = new QLineEdit("0", gridPage);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setStyleSheet("font-size: 20px; padding: 5px;");
    gridMainLayout->addWidget(display);

    auto *gridLayout = new QGridLayout();
    const char *calcLabels[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+"
    };
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            gridLayout->addWidget(
                new QPushButton(calcLabels[row * 4 + col], gridPage), row, col);
        }
    }
    gridMainLayout->addLayout(gridLayout);
    gridMainLayout->addStretch();
    m_ui->tabWidget->addTab(gridPage, "GridLayout");

    // Tab 4: FormLayout - form with label+lineedit pairs
    auto *formPage = new QWidget();
    auto *formMainLayout = new QVBoxLayout(formPage);

    auto *formLayout = new QFormLayout();
    formLayout->addRow("Name:", new QLineEdit(formPage));
    formLayout->addRow("Email:", new QLineEdit(formPage));
    formLayout->addRow("Phone:", new QLineEdit(formPage));
    formMainLayout->addLayout(formLayout);
    formMainLayout->addStretch();
    m_ui->tabWidget->addTab(formPage, "FormLayout");
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
