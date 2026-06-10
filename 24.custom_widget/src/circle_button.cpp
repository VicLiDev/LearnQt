#include "circle_button.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QEvent>
#include <cmath>

// ============================================================
// 构造函数：初始化默认属性
// ============================================================
CircleButton::CircleButton(QWidget *parent)
    : QWidget(parent)
    , mText("")
    , mRadius(40)
    , mState(StateNormal)
    , mCheckable(false)
    , mChecked(false)
    , mNormalColor(QColor(70, 130, 180))
    , mHighlightColor(QColor(30, 90, 140))
    , mTextColor(Qt::white)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setCursor(Qt::PointingHandCursor);
}

// ============================================================
// 设置按钮文字
// ============================================================
void CircleButton::setText(const QString &text)
{
    mText = text;
    update();
}

QString CircleButton::text() const
{
    return mText;
}

// ============================================================
// 设置按钮半径
// ============================================================
void CircleButton::setRadius(int radius)
{
    mRadius = radius;
    updateGeometry();
    update();
}

int CircleButton::radius() const
{
    return mRadius;
}

// ============================================================
// 设置颜色
// ============================================================
void CircleButton::setNormalColor(const QColor &color)
{
    mNormalColor = color;
    update();
}

void CircleButton::setHighlightColor(const QColor &color)
{
    mHighlightColor = color;
    update();
}

// ============================================================
// 设置可选中属性
// ============================================================
void CircleButton::setCheckable(bool checkable)
{
    mCheckable = checkable;
}

bool CircleButton::isCheckable() const
{
    return mCheckable;
}

bool CircleButton::isChecked() const
{
    return mChecked;
}

void CircleButton::setChecked(bool checked)
{
    if (mCheckable && mChecked != checked) {
        mChecked = checked;
        mState = mChecked ? StateChecked : StateNormal;
        emit stateChanged(mState);
        update();
    }
}

// ============================================================
// 重写 paintEvent —— 绘制圆形按钮
// ============================================================
void CircleButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算中心点和实际半径
    int w = width();
    int h = height();
    int actualRadius = qMin(w, h) / 2 - 2;

    // 根据状态选择颜色
    QColor bgColor;
    switch (mState) {
    case StateHover:
        bgColor = mNormalColor.lighter(120);
        break;
    case StatePressed:
        bgColor = mHighlightColor.darker(110);
        break;
    case StateChecked:
        bgColor = mHighlightColor;
        break;
    case StateNormal:
    default:
        bgColor = mNormalColor;
        break;
    }

    // 绘制阴影
    QPainterPath shadowPath;
    shadowPath.addEllipse(2, 2, actualRadius * 2, actualRadius * 2);
    painter.fillPath(shadowPath, QColor(0, 0, 0, 40));

    // 绘制圆形背景
    QPainterPath circlePath;
    circlePath.addEllipse(0, 0, actualRadius * 2, actualRadius * 2);

    // 渐变填充
    QRadialGradient gradient(actualRadius, actualRadius, actualRadius);
    gradient.setColorAt(0, bgColor.lighter(130));
    gradient.setColorAt(1, bgColor);
    painter.fillPath(circlePath, gradient);

    // 绘制边框
    QPen pen(bgColor.darker(150), 2);
    painter.setPen(pen);
    painter.drawEllipse(1, 1, actualRadius * 2 - 2, actualRadius * 2 - 2);

    // 绘制文字（居中）
    if (!mText.isEmpty()) {
        painter.setPen(mTextColor);
        QFont font = painter.font();
        font.setPixelSize(qMax(12, actualRadius / 2));
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(0, 0, actualRadius * 2, actualRadius * 2),
                         Qt::AlignCenter, mText);
    }
}

// ============================================================
// 重写尺寸提示
// ============================================================
QSize CircleButton::sizeHint() const
{
    return QSize(mRadius * 2 + 4, mRadius * 2 + 4);
}

QSize CircleButton::minimumSizeHint() const
{
    return QSize(20, 20);
}

// ============================================================
// 鼠标事件处理
// ============================================================
void CircleButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mState = StatePressed;
        update();
    }
    QWidget::mousePressEvent(event);
}

void CircleButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 如果是可选中按钮，切换选中状态
        if (mCheckable) {
            mChecked = !mChecked;
            mState = mChecked ? StateChecked : StateHover;
            emit clicked(mChecked);
        } else {
            mState = StateHover;
            emit clicked(false);
        }
        emit stateChanged(mState);
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void CircleButton::enterEvent(QEvent *)
{
    if (mState != StatePressed && mState != StateChecked) {
        mState = StateHover;
        update();
    }
}

void CircleButton::leaveEvent(QEvent *)
{
    if (mState != StateChecked) {
        mState = StateNormal;
        update();
    }
}
