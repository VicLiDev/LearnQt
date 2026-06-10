#include "mainwindow.h"

#include <QApplication>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRadialGradient>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QKeyEvent>

// ============================================================
// PaintArea implementation
// ============================================================

PaintArea::PaintArea(QWidget *parent)
    : QWidget(parent)
    , m_radius(60)
{
    setMinimumSize(300, 250);
    setMouseTracking(true);
}

int PaintArea::circleRadius() const
{
    return m_radius;
}

void PaintArea::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), QColor(240, 240, 245));

    // Draw the circle
    int cx = width() / 2;
    int cy = height() / 2;
    QRadialGradient gradient(cx, cy, m_radius);
    gradient.setColorAt(0.0, QColor(100, 180, 255));
    gradient.setColorAt(1.0, QColor(30, 80, 180));
    painter.setBrush(gradient);
    painter.setPen(QPen(Qt::darkBlue, 2));
    painter.drawEllipse(QPoint(cx, cy), m_radius, m_radius);

    // Draw radius text
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(cx - 30, cy + 4, tr("r = %1").arg(m_radius));
}

void PaintArea::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();
    if (delta.y() > 0) {
        m_radius = qMin(m_radius + 10, MAX_RADIUS);
    } else {
        m_radius = qMax(m_radius - 10, MIN_RADIUS);
    }
    event->accept();
    update();
}

// ============================================================
// MainWindow implementation
// ============================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_paintArea(nullptr)
    , m_mouseLabel(nullptr)
    , m_keyLabel(nullptr)
    , m_wheelLabel(nullptr)
    , m_filterLabel(nullptr)
{
    setWindowTitle(tr("Demo 06 - Events"));
    resize(500, 500);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    central->setLayout(mainLayout);

    // Paint area
    m_paintArea = new PaintArea(this);
    mainLayout->addWidget(m_paintArea);

    // Info labels in a grid
    QGridLayout *infoLayout = new QGridLayout();

    infoLayout->addWidget(new QLabel(tr("Mouse:"), this), 0, 0);
    m_mouseLabel = new QLabel(tr("(0, 0)"), this);
    m_mouseLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLayout->addWidget(m_mouseLabel, 0, 1);

    infoLayout->addWidget(new QLabel(tr("Key:"), this), 1, 0);
    m_keyLabel = new QLabel(tr("None"), this);
    m_keyLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLayout->addWidget(m_keyLabel, 1, 1);

    infoLayout->addWidget(new QLabel(tr("Wheel:"), this), 2, 0);
    m_wheelLabel = new QLabel(tr("radius = 60"), this);
    m_wheelLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLayout->addWidget(m_wheelLabel, 2, 1);

    infoLayout->addWidget(new QLabel(tr("Tab Filter:"), this), 3, 0);
    m_filterLabel = new QLabel(tr("(press Tab to test)"), this);
    m_filterLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    infoLayout->addWidget(m_filterLabel, 3, 1);

    mainLayout->addLayout(infoLayout);

    // Install event filter on the paint area for mouse/wheel tracking
    m_paintArea->installEventFilter(this);

    // Install event filter on labels to intercept Tab key
    m_mouseLabel->installEventFilter(this);
    m_keyLabel->installEventFilter(this);
    m_wheelLabel->installEventFilter(this);
    m_filterLabel->installEventFilter(this);

    // Set focus policy so the main window can receive key events
    setFocusPolicy(Qt::StrongFocus);
    central->setFocusPolicy(Qt::ClickFocus);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Build a description of the key press
    QString keyName;
    if (event->key() >= Qt::Key_Space && event->key() <= Qt::Key_AsciiTilde) {
        keyName = QChar(event->key());
    } else {
        keyName = QKeySequence(event->key()).toString();
    }

    QString modifiers;
    Qt::KeyboardModifiers mods = event->modifiers();
    if (mods & Qt::ShiftModifier)
        modifiers += tr("Shift+");
    if (mods & Qt::ControlModifier)
        modifiers += tr("Ctrl+");
    if (mods & Qt::AltModifier)
        modifiers += tr("Alt+");

    m_keyLabel->setText(tr("Key: %1%2 (code=0x%3)")
        .arg(modifiers).arg(keyName)
        .arg(event->key(), 0, 16));

    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // Intercept Tab key presses on child widgets
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            QString widgetName = watched->objectName();
            if (widgetName.isEmpty()) {
                widgetName = tr("unnamed widget");
            }
            m_filterLabel->setText(
                tr("Tab intercepted on: %1 (event consumed)").arg(widgetName));
            return true; // eat the event, prevent propagation
        }
    }

    // Track mouse move over the paint area via event filter
    if (watched == m_paintArea && event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint pos = mouseEvent->pos();
        m_mouseLabel->setText(tr("(%1, %2)").arg(pos.x()).arg(pos.y()));
        // Return false to allow normal event propagation
    }

    // Track wheel events on the paint area
    if (watched == m_paintArea && event->type() == QEvent::Wheel) {
        m_wheelLabel->setText(tr("radius = %1").arg(m_paintArea->circleRadius()));
        // Return false to allow normal event propagation
    }

    return QMainWindow::eventFilter(watched, event);
}

const int PaintArea::MIN_RADIUS;
const int PaintArea::MAX_RADIUS;
