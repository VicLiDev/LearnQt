#ifndef STAR_DELEGATE_H
#define STAR_DELEGATE_H

#include <QStyledItemDelegate>

// ============================================================
// 自定义委托 —— 支持进度条、星评、颜色块三种渲染模式
// 继承 QStyledItemDelegate，重写 paint() 和 sizeHint()
// 实现编辑功能 createEditor() / setModelData()
// ============================================================
class StarDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // 列类型枚举
    enum ColumnType {
        TypeProgress,  // 进度条
        TypeStar,      // 星级评分
        TypeColor      // 颜色块
    };

    explicit StarDelegate(QObject *parent = nullptr);

    // 设置各列的渲染类型
    void setColumnType(int column, ColumnType type);

    // 重写绘制函数
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // 重写尺寸提示
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    // 创建编辑器（星级评分用 QSpinBox）
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    // 设置编辑器数据
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override;

    // 将编辑器数据写回模型
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    // 更新编辑器几何位置
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

private:
    // 各列的类型映射
    QMap<int, ColumnType> mColumnTypes;

    // 绘制进度条
    void paintProgress(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

    // 绘制星级评分
    void paintStars(QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;

    // 绘制颜色块
    void paintColorBlock(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

    // 绘制五角星路径
    QPainterPath starPath(const QPointF &center, double radius) const;
};

#endif // STAR_DELEGATE_H
