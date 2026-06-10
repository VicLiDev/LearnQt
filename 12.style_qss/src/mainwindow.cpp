#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStyleFactory>
#include <QFile>
#include <QComboBox>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    populateStyleComboBox();

    connect(ui->styleComboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
            this, &MainWindow::onStyleChanged);

    connect(ui->darkThemeBtn, &QPushButton::clicked, this, &MainWindow::onLoadDarkTheme);
    connect(ui->lightThemeBtn, &QPushButton::clicked, this, &MainWindow::onLoadLightTheme);

    // Sync slider and spinbox
    connect(ui->volumeSlider, &QSlider::valueChanged, ui->volumeSpinBox, &QSpinBox::setValue);
    connect(ui->volumeSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            ui->volumeSlider, &QSlider::setValue);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateStyleComboBox()
{
    ui->styleComboBox->addItems(QStyleFactory::keys());

    // Set current style as selected
    QString currentStyle = QApplication::style()->objectName();
    int index = ui->styleComboBox->findText(currentStyle, Qt::MatchContains);
    if (index >= 0) {
        ui->styleComboBox->setCurrentIndex(index);
    }
}

void MainWindow::onStyleChanged(const QString &styleName)
{
    applyStyle(styleName);
}

void MainWindow::applyStyle(const QString &styleName)
{
    QStyle *style = QStyleFactory::create(styleName);
    if (style) {
        QApplication::setStyle(style);
        statusBar()->showMessage(QString("Style changed to: %1").arg(styleName), 3000);
    }
}

void MainWindow::onLoadDarkTheme()
{
    QFile file(":/style.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stylesheet = QString::fromUtf8(file.readAll());
        qApp->setStyleSheet(stylesheet);
        file.close();
        statusBar()->showMessage("Dark theme loaded from resource", 3000);
    } else {
        statusBar()->showMessage("Failed to load dark theme from resource", 3000);
    }
}

void MainWindow::onLoadLightTheme()
{
    qApp->setStyleSheet(QString());
    statusBar()->showMessage("Light theme (default) applied", 3000);
}
