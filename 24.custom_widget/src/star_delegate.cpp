#include "star_delegate.h"

#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QApplication>
#include <QMap>
#include <QtMath>

// ============================================================
// 构造函数
// ============================================================
StarDelegate::StarDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// ============================================================
// 设置列类型
// ============================================================
void StarDelegate::setColumnType(int column, ColumnType type)
{
    mColumnTypes[column] = type;
}

// ============================================================
// 重写 paint —— 根据列类型选择绘制方式
// ============================================================
void StarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    // 确保有列类型设置，否则使用默认绘制
    if (!mColumnTypes.contains(index.column())) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制选中/悬停背景
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    switch (mColumnTypes[index.column()]) {
    case TypeProgress:
        paintProgress(painter, option, index);
        break;
    case TypeStar:
        paintStars(painter, option, index);
        break;
    case TypeColor:
        paintColorBlock(painter, option, index);
        break;
    default:
        QStyledItemDelegate::paint(painter, option, index);
        break;
    }

    painter->restore();
}

// ============================================================
// 重写 sizeHint
// ============================================================
QSize StarDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (mColumnTypes.contains(index.column())) {
        return QSize(option.rect.width(), 36);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

// ============================================================
// 创建编辑器 —— 星级评分列使用 QSpinBox
// ============================================================
QWidget *StarDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    if (mColumnTypes.value(index.column()) == TypeStar) {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setRange(0, 5);
        spinBox->setFrame(false);
        return spinBox;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

// ============================================================
// 设置编辑器数据
// ============================================================
void StarDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
    if (mColumnTypes.value(index.column()) == TypeStar) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->setValue(index.data(Qt::EditRole).toInt());
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

// ============================================================
// 将编辑器数据写回模型
// ============================================================
void StarDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    if (mColumnTypes.value(index.column()) == TypeStar) {
        QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
        spinBox->interpretText();
        model->setData(index, spinBox->value(), Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

// ============================================================
// 更新编辑器几何位置
// ============================================================
void StarDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

// ============================================================
// 绘制进度条
// ============================================================
void StarDelegate::paintProgress(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    bool ok = false;
    int value = index.data(Qt::DisplayRole).toInt(&ok);
    if (!ok) value = 0;

    // 边距
    int margin = 6;
    QRect barRect = option.rect.adjusted(margin, margin + 6, -margin, -(margin + 6));

    // 背景槽
    QPen borderPen(QColor(180, 180, 180), 1);
    painter->setPen(borderPen);
    painter->setBrush(QColor(230, 230, 230));
    painter->drawRoundedRect(barRect, 3, 3);

    // 进度条（根据进度变色）
    int fillWidth = static_cast<int>(barRect.width() * qMin(value, 100) / 100.0);
    if (fillWidth > 0) {
        QRect fillRect(barRect.x(), barRect.y(), fillWidth, barRect.height());
        QColor fillColor;
        if (value < 30) {
            fillColor = QColor(220, 60, 60);    // 红色：低进度
        } else if (value < 70) {
            fillColor = QColor(240, 180, 30);  // 橙色：中进度
        } else {
            fillColor = QColor(60, 180, 80);   // 绿色：高进度
        }

        QLinearGradient gradient(fillRect.topLeft(), fillRect.bottomLeft());
        gradient.setColorAt(0, fillColor.lighter(120));
        gradient.setColorAt(1, fillColor);
        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawRoundedRect(fillRect, 3, 3);
    }

    // 百分比文字
    painter->setPen(option.state & QStyle::State_Selected ? Qt::white : Qt::black);
    QFont font = painter->font();
    font.setPixelSize(12);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(barRect, Qt::AlignCenter, QString::number(value) + "%");
}

// ============================================================
// 绘制星级评分
// ============================================================
void StarDelegate::paintStars(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    bool ok = false;
    int starCount = index.data(Qt::DisplayRole).toInt(&ok);
    if (!ok) starCount = 0;
    starCount = qBound(0, starCount, 5);

    int margin = 8;
    int starSize = 16;
    int spacing = 4;
    int totalWidth = starSize * 5 + spacing * 4;
    int startX = option.rect.x() + (option.rect.width() - totalWidth) / 2;
    int centerY = option.rect.y() + option.rect.height() / 2;

    for (int i = 0; i < 5; ++i) {
        QPointF center(startX + i * (starSize + spacing) + starSize / 2.0,
                       centerY);

        if (i < starCount) {
            // 实心星（金色）
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 200, 50));
        } else {
            // 空心星（灰色）
            painter->setPen(QPen(QColor(180, 180, 180), 1));
            painter->setBrush(QColor(220, 220, 220));
        }

        QPainterPath path = starPath(center, starSize / 2.0);
        painter->drawPath(path);
    }
}

// ============================================================
// 绘制颜色块
// ============================================================
void StarDelegate::paintColorBlock(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QColor color = index.data(Qt::DisplayRole).value<QColor>();

    int margin = 8;
    QRect colorRect = option.rect.adjusted(margin, margin, -margin, -margin);

    // 绘制颜色块
    painter->setPen(QPen(color.darker(120), 1));
    painter->setBrush(color);
    painter->drawRoundedRect(colorRect, 4, 4);

    // 在颜色块上显示十六进制颜色值
    QString hexStr = color.name();
    // 根据亮度选择文字颜色
    int brightness = color.red() * 299 + color.green() * 587 + color.blue() * 114;
    painter->setPen(brightness > 128000 ? Qt::black : Qt::white);
    QFont font = painter->font();
    font.setPixelSize(10);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(colorRect, Qt::AlignCenter, hexStr);
}

// ============================================================
// 生成五角星路径
// ============================================================
QPainterPath StarDelegate::starPath(const QPointF &center, double radius) const
{
    QPainterPath path;
    double innerRadius = radius * 0.4; // 内半径

    for (int i = 0; i < 5; ++i) {
        // 外顶点
        double outerAngle = -90.0 + i * 72.0;
        double outerRad = qDegreesToRadians(outerAngle);
        QPointF outerPoint(center.x() + radius * qCos(outerRad),
                           center.y() + radius * qSin(outerRad));

        if (i == 0) {
            path.moveTo(outerPoint);
        } else {
            path.lineTo(outerPoint);
        }

        // 内顶点
        double innerAngle = outerAngle + 36.0;
        double innerRad = qDegreesToRadians(innerAngle);
        QPointF innerPoint(center.x() + innerRadius * qCos(innerRad),
                           center.y() + innerRadius * qSin(innerRad));
        path.lineTo(innerPoint);
    }
    path.closeSubpath();
    return path;
}
