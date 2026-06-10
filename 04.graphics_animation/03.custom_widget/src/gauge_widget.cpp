#include "gauge_widget.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QtMath>

// ============================================================
// 构造函数：初始化默认属性
// ============================================================
GaugeWidget::GaugeWidget(QWidget *parent)
    : QWidget(parent)
    , mValue(0)
    , mMinValue(0)
    , mMaxValue(100)
    , mArcColor(QColor(70, 180, 120))
    , mNeedleColor(QColor(220, 50, 50))
    , mTextColor(Qt::white)
    , mBgColor(QColor(50, 50, 60))
    , mAnimStartValue(0)
    , mAnimEndValue(0)
    , mAnimSteps(30)
    , mAnimCurrentStep(0)
    , mAnimTimer(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建动画定时器
    mAnimTimer = new QTimer(this);
    mAnimTimer->setInterval(20); // 20ms 间隔，约 50fps
    connect(mAnimTimer, SIGNAL(timeout()), this, SLOT(onAnimationStep()));
}

// ============================================================
// 属性访问函数
// ============================================================
double GaugeWidget::value() const { return mValue; }
double GaugeWidget::minValue() const { return mMinValue; }
double GaugeWidget::maxValue() const { return mMaxValue; }
QColor GaugeWidget::arcColor() const { return mArcColor; }
QColor GaugeWidget::needleColor() const { return mNeedleColor; }
QColor GaugeWidget::textColor() const { return mTextColor; }

// ============================================================
// 设置当前值
// ============================================================
void GaugeWidget::setValue(double value)
{
    // 将值限制在范围内
    value = qMax(mMinValue, qMin(mMaxValue, value));
    if (!qFuzzyCompare(mValue, value)) {
        mValue = value;
        emit valueChanged(mValue);
        update();
    }
}

// ============================================================
// 设置范围
// ============================================================
void GaugeWidget::setRange(double minVal, double maxVal)
{
    mMinValue = minVal;
    mMaxValue = maxVal;
    // 确保当前值在范围内
    setValue(mValue);
    update();
}

// ============================================================
// 设置颜色
// ============================================================
void GaugeWidget::setArcColor(const QColor &color)
{
    mArcColor = color;
    update();
}

void GaugeWidget::setNeedleColor(const QColor &color)
{
    mNeedleColor = color;
    update();
}

void GaugeWidget::setTextColor(const QColor &color)
{
    mTextColor = color;
    update();
}

// ============================================================
// 动画：渐变到目标值
// ============================================================
void GaugeWidget::animateToValue(double targetValue)
{
    targetValue = qMax(mMinValue, qMin(mMaxValue, targetValue));
    mAnimStartValue = mValue;
    mAnimEndValue = targetValue;
    mAnimCurrentStep = 0;
    mAnimTimer->start();
}

// ============================================================
// 动画步进
// ============================================================
void GaugeWidget::onAnimationStep()
{
    mAnimCurrentStep++;
    if (mAnimCurrentStep >= mAnimSteps) {
        // 动画结束，设置最终值
        mAnimTimer->stop();
        setValue(mAnimEndValue);
    } else {
        // 线性插值计算当前值
        double progress = static_cast<double>(mAnimCurrentStep) / mAnimSteps;
        // 使用缓出曲线使动画更自然
        double eased = 1.0 - (1.0 - progress) * (1.0 - progress);
        double currentVal = mAnimStartValue + (mAnimEndValue - mAnimStartValue) * eased;
        setValue(currentVal);
    }
}

// ============================================================
// 将值转换为角度
// 仪表盘半圆：最小值对应 180°（左侧），最大值对应 0°（右侧）
// ============================================================
double GaugeWidget::valueToAngle(double val) const
{
    double ratio = (val - mMinValue) / (mMaxValue - mMinValue);
    return 180.0 - ratio * 180.0;
}

// ============================================================
// 重写 paintEvent —— 绘制仪表盘
// ============================================================
void GaugeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    int centerX = width() / 2;
    int centerY = side / 2 + side / 6;
    int outerRadius = side / 2 - 10;

    // ---- 1. 绘制背景圆弧（底色） ----
    QPen bgPen(mBgColor, 16, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(bgPen);
    QRectF arcRect(centerX - outerRadius, centerY - outerRadius,
                   outerRadius * 2, outerRadius * 2);
    // 从 180° 到 0° 画半圆
    painter.drawArc(arcRect, 180 * 16, -180 * 16);

    // ---- 2. 绘制进度圆弧 ----
    double currentAngle = valueToAngle(mValue);
    int spanAngle = static_cast<int>((180.0 - currentAngle) * 16);
    QPen arcPen(mArcColor, 16, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(arcPen);
    if (spanAngle > 0) {
        painter.drawArc(arcRect, 180 * 16, -spanAngle);
    }

    // ---- 3. 绘制刻度线 ----
    painter.save();
    painter.translate(centerX, centerY);
    int majorTickCount = 10;  // 主刻度数量
    int minorTicksPerMajor = 5; // 每个主刻度间的小刻度数

    for (int i = 0; i <= majorTickCount; ++i) {
        double angle = 180.0 - (i * 180.0 / majorTickCount);
        double rad = qDegreesToRadians(angle);

        // 主刻度
        int innerR = outerRadius - 24;
        int outerR = outerRadius - 12;
        QPen tickPen(mTextColor, 2);
        painter.setPen(tickPen);
        painter.drawLine(
            static_cast<int>(innerR * qCos(rad)),
            static_cast<int>(-innerR * qSin(rad)),
            static_cast<int>(outerR * qCos(rad)),
            static_cast<int>(-outerR * qSin(rad))
        );

        // 小刻度
        if (i < majorTickCount) {
            for (int j = 1; j < minorTicksPerMajor; ++j) {
                double minorAngle = angle - (j * 180.0) / (majorTickCount * minorTicksPerMajor);
                double minorRad = qDegreesToRadians(minorAngle);
                int minorInnerR = outerRadius - 20;
                QPen minorPen(mTextColor.darker(150), 1);
                painter.setPen(minorPen);
                painter.drawLine(
                    static_cast<int>(minorInnerR * qCos(minorRad)),
                    static_cast<int>(-minorInnerR * qSin(minorRad)),
                    static_cast<int>(outerR * qCos(minorRad)),
                    static_cast<int>(-outerR * qSin(minorRad))
                );
            }
        }

        // 刻度文字
        double labelR = outerRadius - 36;
        double labelVal = mMinValue + (i * (mMaxValue - mMinValue) / majorTickCount);
        QPen labelPen(mTextColor);
        painter.setPen(labelPen);
        QFont labelFont = painter.font();
        labelFont.setPixelSize(qMax(8, side / 20));
        painter.setFont(labelFont);
        QRectF labelRect(0, 0, side / 8, side / 10);
        QPointF labelCenter(labelR * qCos(rad), -labelR * qSin(rad));
        labelRect.moveCenter(labelCenter);
        painter.drawText(labelRect, Qt::AlignCenter, QString::number(static_cast<int>(labelVal)));
    }
    painter.restore();

    // ---- 4. 绘制指针 ----
    painter.save();
    painter.translate(centerX, centerY);
    double needleAngle = valueToAngle(mValue);
    double needleRad = qDegreesToRadians(needleAngle);

    // 指针三角形
    int needleLength = outerRadius - 30;
    int needleWidth = 6;
    QPainterPath needlePath;
    needlePath.moveTo(needleLength * qCos(needleRad),
                      -needleLength * qSin(needleRad));
    needlePath.lineTo(needleWidth * qCos(needleRad + M_PI / 2),
                      -needleWidth * qSin(needleRad + M_PI / 2));
    needlePath.lineTo(needleWidth * qCos(needleRad - M_PI / 2),
                      -needleWidth * qSin(needleRad - M_PI / 2));
    needlePath.closeSubpath();

    QPen needlePen(mNeedleColor.darker(120), 1);
    painter.setPen(needlePen);
    painter.fillPath(needlePath, mNeedleColor);

    // 指针中心圆
    painter.setPen(Qt::NoPen);
    painter.setBrush(mNeedleColor.darker(120));
    painter.drawEllipse(QPointF(0, 0), 8, 8);
    painter.restore();

    // ---- 5. 绘制数值文字 ----
    QPen textPen(mTextColor);
    painter.setPen(textPen);
    QFont valueFont = painter.font();
    valueFont.setPixelSize(qMax(14, side / 6));
    valueFont.setBold(true);
    painter.setFont(valueFont);
    QRectF valueRect(centerX - side / 4, centerY + 10, side / 2, side / 6);
    painter.drawText(valueRect, Qt::AlignCenter, QString::number(mValue, 'f', 1));
}

// ============================================================
// 重写尺寸提示
// ============================================================
QSize GaugeWidget::sizeHint() const
{
    return QSize(250, 200);
}

QSize GaugeWidget::minimumSizeHint() const
{
    return QSize(150, 120);
}
