#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
    , m_statusLabel(nullptr)
{
    m_ui->setupUi(this);
    m_statusLabel = m_ui->statusLabel;
    setupWidgets();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupWidgets()
{
    auto *layout = m_ui->scrollLayout;

    // QLabel - static text
    auto *staticLabel = new QLabel(
        "<b>Qt Basic Widgets Demo</b><br/>"
        "This page demonstrates common Qt widgets.<br/>"
        "Interact with any widget and see the response below.",
        m_ui->scrollContents);
    staticLabel->setWordWrap(true);
    staticLabel->setFrameShape(QFrame::StyledPanel);
    layout->addWidget(staticLabel);

    // QLineEdit - single line text input
    auto *lineEdit = new QLineEdit(m_ui->scrollContents);
    lineEdit->setPlaceholderText("Type something here...");
    layout->addWidget(new QLabel("QLineEdit:", m_ui->scrollContents));
    layout->addWidget(lineEdit);
    connect(lineEdit, &QLineEdit::textChanged,
            this, &MainWindow::onLineEditChanged);

    // QTextEdit - multi-line text editor
    layout->addWidget(new QLabel("QTextEdit:", m_ui->scrollContents));
    auto *textEdit = new QTextEdit(m_ui->scrollContents);
    textEdit->setMaximumHeight(80);
    textEdit->setPlaceholderText("Multi-line text editor...");
    layout->addWidget(textEdit);
    connect(textEdit, &QTextEdit::textChanged, this, [this, textEdit]() {
        m_statusLabel->setText(
            QString("QTextEdit changed (%1 chars)")
                .arg(textEdit->toPlainText().length()));
    });

    // QSpinBox and QDoubleSpinBox
    auto *spinBoxLayout = new QHBoxLayout();
    auto *spinBox = new QSpinBox(m_ui->scrollContents);
    spinBox->setRange(0, 100);
    spinBox->setValue(42);
    auto *doubleSpinBox = new QDoubleSpinBox(m_ui->scrollContents);
    doubleSpinBox->setRange(0.0, 1000.0);
    doubleSpinBox->setValue(3.14);
    doubleSpinBox->setDecimals(2);
    spinBoxLayout->addWidget(new QLabel("QSpinBox:", m_ui->scrollContents));
    spinBoxLayout->addWidget(spinBox);
    spinBoxLayout->addWidget(new QLabel("QDoubleSpinBox:", m_ui->scrollContents));
    spinBoxLayout->addWidget(doubleSpinBox);
    layout->addLayout(spinBoxLayout);
    connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onSpinBoxValueChanged);
    connect(doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onDoubleSpinBoxValueChanged);

    // QComboBox - non-editable
    auto *comboBox = new QComboBox(m_ui->scrollContents);
    comboBox->addItems(QStringList() << "Apple" << "Banana" << "Cherry" << "Date");
    layout->addWidget(new QLabel("QComboBox (non-editable):", m_ui->scrollContents));
    layout->addWidget(comboBox);
    connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::onComboBoxIndexChanged);

    // QComboBox - editable
    auto *editComboBox = new QComboBox(m_ui->scrollContents);
    editComboBox->setEditable(true);
    editComboBox->addItems(QStringList() << "Red" << "Green" << "Blue");
    layout->addWidget(new QLabel("QComboBox (editable):", m_ui->scrollContents));
    layout->addWidget(editComboBox);
    connect(editComboBox, &QComboBox::editTextChanged,
            this, &MainWindow::onComboBoxEditTextChanged);

    // QRadioButton - exclusive group
    auto *radioGroup = new QGroupBox("QRadioButton (exclusive)", m_ui->scrollContents);
    auto *radioLayout = new QHBoxLayout(radioGroup);
    auto *radio1 = new QRadioButton("Option A", radioGroup);
    auto *radio2 = new QRadioButton("Option B", radioGroup);
    auto *radio3 = new QRadioButton("Option C", radioGroup);
    radio1->setChecked(true);
    radioLayout->addWidget(radio1);
    radioLayout->addWidget(radio2);
    radioLayout->addWidget(radio3);
    layout->addWidget(radioGroup);

    auto radioMapper = [this](QRadioButton *btn) {
        connect(btn, &QRadioButton::toggled, this, [this, btn](bool checked) {
            if (checked) {
                m_statusLabel->setText(
                    QString("QRadioButton selected: %1").arg(btn->text()));
            }
        });
    };
    radioMapper(radio1);
    radioMapper(radio2);
    radioMapper(radio3);

    // QCheckBox - non-exclusive
    auto *checkGroup = new QGroupBox("QCheckBox (non-exclusive)", m_ui->scrollContents);
    auto *checkLayout = new QHBoxLayout(checkGroup);
    auto *check1 = new QCheckBox("Enable Feature X", checkGroup);
    auto *check2 = new QCheckBox("Enable Feature Y", checkGroup);
    auto *check3 = new QCheckBox("Enable Feature Z", checkGroup);
    checkLayout->addWidget(check1);
    checkLayout->addWidget(check2);
    checkLayout->addWidget(check3);
    layout->addWidget(checkGroup);

    auto checkMapper = [this](QCheckBox *cb) {
        connect(cb, &QCheckBox::toggled, this, [this, cb](bool checked) {
            m_statusLabel->setText(
                QString("QCheckBox \"%1\" is %2")
                    .arg(cb->text())
                    .arg(checked ? "checked" : "unchecked"));
        });
    };
    checkMapper(check1);
    checkMapper(check2);
    checkMapper(check3);

    // QSlider (horizontal) with QProgressBar
    auto *sliderGroup = new QGroupBox("QSlider and QProgressBar", m_ui->scrollContents);
    auto *sliderLayout = new QVBoxLayout(sliderGroup);

    auto *slider = new QSlider(Qt::Horizontal, sliderGroup);
    slider->setRange(0, 100);
    slider->setValue(30);

    auto *progressBar = new QProgressBar(sliderGroup);
    progressBar->setRange(0, 100);
    progressBar->setValue(slider->value());

    sliderLayout->addWidget(new QLabel("QSlider:", sliderGroup));
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(new QLabel("QProgressBar:", sliderGroup));
    sliderLayout->addWidget(progressBar);
    layout->addWidget(sliderGroup);

    connect(slider, &QSlider::valueChanged, this, [this, progressBar](int value) {
        progressBar->setValue(value);
        onSliderValueChanged(value);
    });

    // QDateEdit and QTimeEdit
    auto *dateTimeLayout = new QHBoxLayout();
    auto *dateEdit = new QDateEdit(m_ui->scrollContents);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    auto *timeEdit = new QTimeEdit(m_ui->scrollContents);
    timeEdit->setTime(QTime::currentTime());
    dateTimeLayout->addWidget(new QLabel("QDateEdit:", m_ui->scrollContents));
    dateTimeLayout->addWidget(dateEdit);
    dateTimeLayout->addWidget(new QLabel("QTimeEdit:", m_ui->scrollContents));
    dateTimeLayout->addWidget(timeEdit);
    layout->addLayout(dateTimeLayout);
    connect(dateEdit, &QDateEdit::dateChanged, this, &MainWindow::onDateChanged);
    connect(timeEdit, &QTimeEdit::timeChanged, this, &MainWindow::onTimeChanged);

    layout->addStretch();
}

void MainWindow::onLineEditChanged(const QString &text)
{
    m_statusLabel->setText(QString("QLineEdit text changed: \"%1\"").arg(text));
}

void MainWindow::onSliderValueChanged(int value)
{
    m_statusLabel->setText(QString("QSlider value: %1").arg(value));
}

void MainWindow::onSpinBoxValueChanged(int value)
{
    m_statusLabel->setText(QString("QSpinBox value: %1").arg(value));
}

void MainWindow::onDoubleSpinBoxValueChanged(double value)
{
    m_statusLabel->setText(QString("QDoubleSpinBox value: %1").arg(value));
}

void MainWindow::onComboBoxIndexChanged(int index)
{
    m_statusLabel->setText(QString("QComboBox index changed to: %1").arg(index));
}

void MainWindow::onComboBoxEditTextChanged(const QString &text)
{
    m_statusLabel->setText(QString("QComboBox (editable) text: \"%1\"").arg(text));
}

void MainWindow::onDateChanged(const QDate &date)
{
    m_statusLabel->setText(QString("QDateEdit: %1").arg(date.toString("yyyy-MM-dd")));
}

void MainWindow::onTimeChanged(const QTime &time)
{
    m_statusLabel->setText(QString("QTimeEdit: %1").arg(time.toString("HH:mm:ss")));
}
