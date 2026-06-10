#include "mainwindow.h"
#include "aboutdialog.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_logEdit(nullptr)
    , m_colorLabel(nullptr)
{
    ui->setupUi(this);
    createWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createWidgets()
{
    QVBoxLayout *mainLayout = static_cast<QVBoxLayout *>(ui->centralWidget->layout());

    // Color preview label
    m_colorLabel = new QLabel(tr("Color Preview"), this);
    m_colorLabel->setAlignment(Qt::AlignCenter);
    m_colorLabel->setAutoFillBackground(true);
    m_colorLabel->setMinimumHeight(40);
    QPalette pal = m_colorLabel->palette();
    pal.setColor(QPalette::Window, Qt::white);
    m_colorLabel->setPalette(pal);
    mainLayout->addWidget(m_colorLabel);

    // File dialogs group
    QGroupBox *fileGroup = new QGroupBox(tr("File Dialogs"), this);
    QGridLayout *fileLayout = new QGridLayout(fileGroup);

    QPushButton *openBtn = new QPushButton(tr("Open File"), this);
    QPushButton *saveBtn = new QPushButton(tr("Save File"), this);
    fileLayout->addWidget(openBtn, 0, 0);
    fileLayout->addWidget(saveBtn, 0, 1);

    connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveFile);
    mainLayout->addWidget(fileGroup);

    // Input dialogs group
    QGroupBox *inputGroup = new QGroupBox(tr("Input Dialogs"), this);
    QGridLayout *inputLayout = new QGridLayout(inputGroup);

    QPushButton *colorBtn = new QPushButton(tr("Select Color"), this);
    QPushButton *textBtn = new QPushButton(tr("Get Text"), this);
    QPushButton *intBtn = new QPushButton(tr("Get Int"), this);
    QPushButton *itemBtn = new QPushButton(tr("Get Item"), this);
    inputLayout->addWidget(colorBtn, 0, 0);
    inputLayout->addWidget(textBtn, 0, 1);
    inputLayout->addWidget(intBtn, 1, 0);
    inputLayout->addWidget(itemBtn, 1, 1);

    connect(colorBtn, &QPushButton::clicked, this, &MainWindow::onSelectColor);
    connect(textBtn, &QPushButton::clicked, this, &MainWindow::onGetText);
    connect(intBtn, &QPushButton::clicked, this, &MainWindow::onGetInt);
    connect(itemBtn, &QPushButton::clicked, this, &MainWindow::onGetItem);
    mainLayout->addWidget(inputGroup);

    // Message dialogs group
    QGroupBox *msgGroup = new QGroupBox(tr("Message Dialogs"), this);
    QGridLayout *msgLayout = new QGridLayout(msgGroup);

    QPushButton *questionBtn = new QPushButton(tr("Question"), this);
    QPushButton *warningBtn = new QPushButton(tr("Warning"), this);
    QPushButton *infoBtn = new QPushButton(tr("Information"), this);
    QPushButton *criticalBtn = new QPushButton(tr("Critical"), this);
    QPushButton *customBtn = new QPushButton(tr("Custom Dialog"), this);
    msgLayout->addWidget(questionBtn, 0, 0);
    msgLayout->addWidget(warningBtn, 0, 1);
    msgLayout->addWidget(infoBtn, 1, 0);
    msgLayout->addWidget(criticalBtn, 1, 1);
    msgLayout->addWidget(customBtn, 2, 0, 1, 2);

    connect(questionBtn, &QPushButton::clicked, this, &MainWindow::onQuestion);
    connect(warningBtn, &QPushButton::clicked, this, &MainWindow::onWarning);
    connect(infoBtn, &QPushButton::clicked, this, &MainWindow::onInformation);
    connect(criticalBtn, &QPushButton::clicked, this, &MainWindow::onCritical);
    connect(customBtn, &QPushButton::clicked, this, &MainWindow::onCustomDialog);
    mainLayout->addWidget(msgGroup);

    // Log area
    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    m_logEdit->setMaximumHeight(150);
    mainLayout->addWidget(new QLabel(tr("Results:"), this));
    mainLayout->addWidget(m_logEdit);
}

void MainWindow::log(const QString &message)
{
    m_logEdit->append(message);
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open File"), QString(),
        tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
    if (!fileName.isEmpty()) {
        log(tr("[OpenFile] Selected: %1").arg(fileName));
    } else {
        log(tr("[OpenFile] Cancelled"));
    }
}

void MainWindow::onSaveFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save File"), QString(),
        tr("All Files (*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
    if (!fileName.isEmpty()) {
        log(tr("[SaveFile] Selected: %1").arg(fileName));
    } else {
        log(tr("[SaveFile] Cancelled"));
    }
}

void MainWindow::onSelectColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, tr("Select Color"));
    if (color.isValid()) {
        QPalette pal = m_colorLabel->palette();
        pal.setColor(QPalette::Window, color);
        m_colorLabel->setPalette(pal);
        log(tr("[SelectColor] Selected: %1").arg(color.name()));
    } else {
        log(tr("[SelectColor] Cancelled"));
    }
}

void MainWindow::onGetText()
{
    bool ok = false;
    QString text = QInputDialog::getText(
        this, tr("Get Text"), tr("Enter your name:"), QLineEdit::Normal,
        QString(), &ok);
    if (ok) {
        log(tr("[GetText] Entered: \"%1\"").arg(text));
    } else {
        log(tr("[GetText] Cancelled"));
    }
}

void MainWindow::onGetInt()
{
    bool ok = false;
    int value = QInputDialog::getInt(
        this, tr("Get Int"), tr("Enter a number:"), 0, -10000, 10000, 1, &ok);
    if (ok) {
        log(tr("[GetInt] Entered: %1").arg(value));
    } else {
        log(tr("[GetInt] Cancelled"));
    }
}

void MainWindow::onGetItem()
{
    bool ok = false;
    QStringList items;
    items << tr("Red") << tr("Green") << tr("Blue") << tr("Yellow") << tr("Purple");
    QString item = QInputDialog::getItem(
        this, tr("Get Item"), tr("Select a color:"), items, 0, false, &ok);
    if (ok) {
        log(tr("[GetItem] Selected: \"%1\"").arg(item));
    } else {
        log(tr("[GetItem] Cancelled"));
    }
}

void MainWindow::onQuestion()
{
    int ret = QMessageBox::question(
        this, tr("Question"),
        tr("Do you want to proceed?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes);
    QString result;
    switch (ret) {
    case QMessageBox::Yes:
        result = tr("Yes");
        break;
    case QMessageBox::No:
        result = tr("No");
        break;
    case QMessageBox::Cancel:
        result = tr("Cancel");
        break;
    default:
        result = tr("Unknown");
        break;
    }
    log(tr("[Question] User clicked: %1").arg(result));
}

void MainWindow::onWarning()
{
    QMessageBox::warning(
        this, tr("Warning"),
        tr("This is a warning message.\nThe operation may have unexpected results."));
    log(tr("[Warning] Dialog closed"));
}

void MainWindow::onInformation()
{
    QMessageBox::information(
        this, tr("Information"),
        tr("This is an informational message.\nEverything is working correctly."));
    log(tr("[Information] Dialog closed"));
}

void MainWindow::onCritical()
{
    QMessageBox::critical(
        this, tr("Critical"),
        tr("This is a critical error message.\nApplication may need to restart."));
    log(tr("[Critical] Dialog closed"));
}

void MainWindow::onCustomDialog()
{
    AboutDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        log(tr("[CustomDialog] Accepted (OK clicked)"));
    } else {
        log(tr("[CustomDialog] Rejected"));
    }
}
