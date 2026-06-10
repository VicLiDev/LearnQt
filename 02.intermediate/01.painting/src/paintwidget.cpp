#include "paintwidget.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QLinearGradient>
#include <QPen>
#include <QFont>
#include <QTimer>
#include <QtMath>

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent)
    , m_angle(0)
{
    // Timer that fires every 1 second to animate the rotating line
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_angle = (m_angle + 15) % 360;
        update();
    });
    m_timer.start(1000);
}

void PaintWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // ---- Background ----
    painter.fillRect(rect(), Qt::white);

    // ---- Section 1: Lines with different pen styles (left area) ----
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(20, 25, tr("Lines with different pen styles:"));

    int lineY = 45;
    int lineSpacing = 22;

    // Solid line
    QPen solidPen(Qt::darkBlue, 2, Qt::SolidLine);
    painter.setPen(solidPen);
    painter.drawLine(20, lineY, 200, lineY);
    painter.drawText(210, lineY + 4, tr("Solid"));
    lineY += lineSpacing;

    // Dashed line
    QPen dashPen(Qt::darkGreen, 2, Qt::DashLine);
    painter.setPen(dashPen);
    painter.drawLine(20, lineY, 200, lineY);
    painter.drawText(210, lineY + 4, tr("Dash"));
    lineY += lineSpacing;

    // Dotted line
    QPen dotPen(Qt::darkRed, 2, Qt::DotLine);
    painter.setPen(dotPen);
    painter.drawLine(20, lineY, 200, lineY);
    painter.drawText(210, lineY + 4, tr("Dot"));
    lineY += lineSpacing;

    // Dash-dot line
    QPen dashDotPen(Qt::darkCyan, 2, Qt::DashDotLine);
    painter.setPen(dashDotPen);
    painter.drawLine(20, lineY, 200, lineY);
    painter.drawText(210, lineY + 4, tr("DashDot"));
    lineY += lineSpacing;

    // Dash-dot-dot line
    QPen dashDotDotPen(Qt::darkMagenta, 2, Qt::DashDotDotLine);
    painter.setPen(dashDotDotPen);
    painter.drawLine(20, lineY, 200, lineY);
    painter.drawText(210, lineY + 4, tr("DashDotDot"));

    // ---- Section 2: Rectangles (right area, top) ----
    int rectX = 340;
    int rectY = 25;
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(rectX, rectY, tr("Rectangles:"));

    rectY += 15;
    // Filled rectangle
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(180, 220, 255)));
    painter.drawRect(rectX, rectY, 80, 50);

    // Stroked rectangle
    painter.setPen(QPen(Qt::darkBlue, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rectX + 100, rectY, 80, 50);

    // Filled + stroked
    painter.setPen(QPen(Qt::darkRed, 2));
    painter.setBrush(QBrush(QColor(255, 200, 200)));
    painter.drawRect(rectX + 200, rectY, 80, 50);

    // ---- Section 3: Circles / Ellipses (center area) ----
    int ellipseY = rectY + 80;
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(rectX, ellipseY, tr("Ellipses:"));
    ellipseY += 15;

    // Filled ellipse
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(200, 255, 200)));
    painter.drawEllipse(rectX + 40, ellipseY, 80, 50);

    // Stroked ellipse
    painter.setPen(QPen(Qt::darkGreen, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rectX + 150, ellipseY, 80, 50);

    // Circle
    painter.setPen(QPen(Qt::darkYellow, 2));
    painter.setBrush(QBrush(QColor(255, 255, 200)));
    painter.drawEllipse(rectX + 260, ellipseY, 60, 60);

    // ---- Section 4: Arcs and Chords ----
    int arcY = ellipseY + 85;
    painter.setPen(QPen(Qt::black, 1));
    painter.drawText(rectX, arcY, tr("Arcs & Chords:"));
    arcY += 15;

    // Pie
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(QBrush(QColor(255, 180, 180)));
    painter.drawPie(rectX, arcY, 80, 80, 45 * 16, 270 * 16);

    // Chord
    painter.setPen(QPen(Qt::darkBlue, 2));
    painter.setBrush(QBrush(QColor(180, 180, 255)));
    painter.drawChord(rectX + 100, arcY, 80, 80, 30 * 16, 120 * 16);

    // Arc
    painter.setPen(QPen(Qt::darkRed, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(rectX + 200, arcY, 80, 80, 0, 270 * 16);

    // ---- Section 5: Text with custom font ----
    int textY = arcY + 100;
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(rectX, textY, tr("Text with custom fonts:"));
    textY += 20;

    QFont serifFont("Times", 14, QFont::Bold);
    painter.setFont(serifFont);
    painter.setPen(Qt::darkBlue);
    painter.drawText(rectX, textY, tr("Bold Serif 14pt"));

    QFont sansFont("Helvetica", 12, QFont::Normal, true);
    painter.setFont(sansFont);
    painter.setPen(Qt::darkGreen);
    textY += 22;
    painter.drawText(rectX, textY, tr("Italic Sans 12pt"));

    QFont monoFont("Courier", 10);
    painter.setFont(monoFont);
    painter.setPen(Qt::darkRed);
    textY += 22;
    painter.drawText(rectX, textY, tr("Mono 10pt"));

    // ---- Section 6: Gradient-filled rectangle (bottom-left) ----
    int gradY = lineY + 40;
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    QFont defaultFont("Arial", 10);
    painter.setFont(defaultFont);
    painter.setPen(Qt::black);
    painter.drawText(20, gradY, tr("Linear Gradient:"));
    gradY += 10;

    QLinearGradient gradient(20, gradY, 220, gradY + 100);
    gradient.setColorAt(0.0, Qt::red);
    gradient.setColorAt(0.5, Qt::green);
    gradient.setColorAt(1.0, Qt::blue);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRect(20, gradY, 200, 100);

    // ---- Section 7: Rounded rectangle ----
    int roundY = gradY + 120;
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    painter.drawText(20, roundY, tr("Rounded Rectangle:"));
    roundY += 10;

    painter.setPen(QPen(Qt::darkBlue, 3));
    painter.setBrush(QBrush(QColor(220, 220, 255)));
    painter.drawRoundedRect(20, roundY, 200, 80, 20, 15);

    // ---- Section 8: Rotating line (animated, center-bottom) ----
    int rotateX = w / 2 - 60;
    int rotateY = h - 100;

    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.setFont(defaultFont);
    painter.setPen(Qt::black);
    painter.drawText(rotateX, rotateY - 5, tr("Rotating line (1s):"));

    // Draw clock-like circle
    int clockCx = rotateX + 60;
    int clockCy = rotateY + 45;
    int clockR = 40;

    painter.setPen(QPen(Qt::darkGray, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPoint(clockCx, clockCy), clockR, clockR);

    // Draw the rotating hand
    double rad = qDegreesToRadians(static_cast<double>(m_angle) - 90.0);
    int handX = clockCx + static_cast<int>(clockR * 0.8 * qCos(rad));
    int handY = clockCy + static_cast<int>(clockR * 0.8 * qSin(rad));

    painter.setPen(QPen(Qt::red, 3));
    painter.drawLine(clockCx, clockCy, handX, handY);

    // Center dot
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(QPoint(clockCx, clockCy), 4, 4);
}
