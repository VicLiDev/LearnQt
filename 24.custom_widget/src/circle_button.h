#ifndef CIRCLE_BUTTON_H
#define CIRCLE_BUTTON_H

#include <QWidget>

// ============================================================
// 自定义圆形按钮控件
// 继承 QWidget，重写 paintEvent 绘制圆形按钮
// 支持鼠标交互和自定义信号
// ============================================================
class CircleButton : public QWidget
{
    Q_OBJECT

public:
    // 按钮状态枚举
    enum ButtonState {
        StateNormal,    // 正常状态
        StateHover,     // 悬停状态
        StatePressed,   // 按下状态
        StateChecked    // 选中状态
    };

    explicit CircleButton(QWidget *parent = nullptr);

    // 设置按钮文字
    void setText(const QString &text);
    QString text() const;

    // 设置按钮半径
    void setRadius(int radius);
    int radius() const;

    // 设置普通颜色和高亮颜色
    void setNormalColor(const QColor &color);
    void setHighlightColor(const QColor &color);

    // 设置是否可选中
    void setCheckable(bool checkable);
    bool isCheckable() const;

    // 获取当前选中状态
    bool isChecked() const;
    void setChecked(bool checked);

signals:
    // 点击信号
    void clicked(bool checked = false);

    // 状态变化信号
    void stateChanged(int state);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;

    // 重写尺寸提示
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // 鼠标事件处理
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString mText;          // 按钮文字
    int mRadius;             // 按钮半径
    ButtonState mState;     // 当前状态
    bool mCheckable;        // 是否可选中
    bool mChecked;          // 是否选中

    QColor mNormalColor;    // 正常颜色
    QColor mHighlightColor; // 高亮颜色
    QColor mTextColor;      // 文字颜色
};

#endif // CIRCLE_BUTTON_H
