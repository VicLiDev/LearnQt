#ifndef GAUGE_WIDGET_H
#define GAUGE_WIDGET_H

#include <QWidget>

// ============================================================
// 进度仪表盘控件
// 绘制半圆弧仪表盘，包含刻度、指针和数值显示
// 支持设置范围、值、颜色，使用 QTimer 实现动画
// ============================================================
class GaugeWidget : public QWidget
{
    Q_OBJECT

    // 自定义属性，可用于动画
    Q_PROPERTY(double value READ value WRITE setValue)

public:
    explicit GaugeWidget(QWidget *parent = nullptr);

    // 获取/设置当前值
    double value() const;

    // 获取/设置范围（最小值、最大值）
    double minValue() const;
    double maxValue() const;

    // 获取/设置颜色
    QColor arcColor() const;
    QColor needleColor() const;
    QColor textColor() const;

public slots:
    // 设置当前值
    void setValue(double value);

    // 设置范围
    void setRange(double minVal, double maxVal);

    // 设置颜色
    void setArcColor(const QColor &color);
    void setNeedleColor(const QColor &color);
    void setTextColor(const QColor &color);

    // 动画：从当前值渐变到目标值
    void animateToValue(double targetValue);

signals:
    // 值变化信号
    void valueChanged(double value);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;

    // 重写尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    // 当前值
    double mValue;

    // 范围
    double mMinValue;
    double mMaxValue;

    // 颜色
    QColor mArcColor;      // 弧线颜色
    QColor mNeedleColor;  // 指针颜色
    QColor mTextColor;    // 文字颜色
    QColor mBgColor;      // 背景颜色

    // 动画相关
    double mAnimStartValue;   // 动画起始值
    double mAnimEndValue;     // 动画目标值
    int mAnimSteps;           // 动画总步数
    int mAnimCurrentStep;     // 当前动画步数

    // 动画定时器
    QTimer *mAnimTimer;

    // 将值转换为角度（起始角度 180°，终点角度 0°）
    double valueToAngle(double val) const;

private slots:
    // 动画步进槽函数
    void onAnimationStep();
};

#endif // GAUGE_WIDGET_H
