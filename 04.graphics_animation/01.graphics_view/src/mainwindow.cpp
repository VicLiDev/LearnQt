#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QTabWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>
#include <QPen>
#include <QMenu>
#include <QMessageBox>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTransform>
#include <QGraphicsSceneContextMenuEvent>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// FlowNodeItem - 可拖拽流程图节点
// ============================================================================

const int FlowNodeItem::PORT_RADIUS = 6;

FlowNodeItem::FlowNodeItem(const QString &title, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_title(title)
    , m_hovered(false)
{
    // 随机颜色
    int hue = qrand() % 360;
    m_color = QColor::fromHsv(hue, 120, 200);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
}

QRectF FlowNodeItem::boundingRect() const
{
    return QRectF(-60, -25, 120, 50);
}

void FlowNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    // 节点阴影
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 40));
    painter->drawRoundedRect(-57, -22, 120, 50, 8, 8);

    // 节点主体
    QPen pen(Qt::black, m_hovered ? 2.5 : 1.5);
    if (isSelected()) {
        pen.setColor(QColor(0, 120, 215));
        pen.setWidth(2.5);
    }
    painter->setPen(pen);
    painter->setBrush(m_color);
    painter->drawRoundedRect(-60, -25, 120, 50, 8, 8);

    // 标题文字
    painter->setPen(Qt::white);
    painter->setFont(QFont("sans-serif", 10, QFont::Bold));
    painter->drawText(boundingRect(), Qt::AlignCenter, m_title);

    // 输入端口（左侧）
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QColor(100, 200, 100));
    painter->drawEllipse(-60 - PORT_RADIUS, -PORT_RADIUS, PORT_RADIUS * 2, PORT_RADIUS * 2);

    // 输出端口（右侧）
    painter->setBrush(QColor(200, 100, 100));
    painter->drawEllipse(60 - PORT_RADIUS, -PORT_RADIUS, PORT_RADIUS * 2, PORT_RADIUS * 2);
}

QPointF FlowNodeItem::getInputPortPos() const
{
    return mapToScene(QPointF(-60, 0));
}

QPointF FlowNodeItem::getOutputPortPos() const
{
    return mapToScene(QPointF(60, 0));
}

void FlowNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    m_hovered = true;
    update();
}

void FlowNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    m_hovered = false;
    update();
}

QVariant FlowNodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        // 通知场景中的连接线更新
        foreach (QGraphicsItem *item, scene()->items()) {
            if (item->type() == UserType + 2) {
                ConnectionLineItem *line = static_cast<ConnectionLineItem *>(item);
                line->updatePath();
            }
        }
    }
    return QGraphicsObject::itemChange(change, value);
}

// ============================================================================
// ConnectionLineItem - 连接线
// ============================================================================

ConnectionLineItem::ConnectionLineItem(FlowNodeItem *from, FlowNodeItem *to, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_fromNode(from)
    , m_toNode(to)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setZValue(-1); // 在节点下方
    updatePath();
}

QRectF ConnectionLineItem::boundingRect() const
{
    return m_path.boundingRect().adjusted(-5, -5, 5, 5);
}

void ConnectionLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor(80, 80, 80), isSelected() ? 3 : 2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(m_path);

    // 箭头
    QPointF end = m_path.pointAtPercent(1.0);
    QPointF before = m_path.pointAtPercent(0.95);
    QLineF arrowLine(before, end);
    double angle = arrowLine.angle();
    QPointF arrowP1 = end - QPointF(10 * std::cos(angle * M_PI / 180.0 + M_PI / 6.0),
                                       10 * std::sin(angle * M_PI / 180.0 + M_PI / 6.0));
    QPointF arrowP2 = end - QPointF(10 * std::cos(angle * M_PI / 180.0 - M_PI / 6.0),
                                       10 * std::sin(angle * M_PI / 180.0 - M_PI / 6.0));
    QPolygonF arrow;
    arrow << end << arrowP1 << arrowP2;
    painter->setBrush(QColor(80, 80, 80));
    painter->drawPolygon(arrow);
}

void ConnectionLineItem::updatePath()
{
    if (!m_fromNode || !m_toNode) return;

    QPointF startPos = m_fromNode->getOutputPortPos();
    QPointF endPos = m_toNode->getInputPortPos();

    m_path = QPainterPath();
    m_path.moveTo(startPos);

    // 贝塞尔曲线连接
    double dx = endPos.x() - startPos.x();
    QPointF ctrl1(startPos.x() + dx * 0.5, startPos.y());
    QPointF ctrl2(endPos.x() - dx * 0.5, endPos.y());
    m_path.cubicTo(ctrl1, ctrl2, endPos);

    prepareGeometryChange();
    update();
}

QVariant ConnectionLineItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsObject::itemChange(change, value);
}

// ============================================================================
// Tab1: 基本图形
// ============================================================================

BasicShapesTab::BasicShapesTab(QWidget *parent)
    : QWidget(parent)
    , m_lastRotation(0)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 控制面板
    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    QPushButton *btnRect = new QPushButton("添加矩形");
    QPushButton *btnEllipse = new QPushButton("添加椭圆");
    QPushButton *btnText = new QPushButton("添加文本");
    QPushButton *btnLine = new QPushButton("添加线条");
    QPushButton *btnClear = new QPushButton("清空");
    ctrlLayout->addWidget(btnRect);
    ctrlLayout->addWidget(btnEllipse);
    ctrlLayout->addWidget(btnText);
    ctrlLayout->addWidget(btnLine);
    ctrlLayout->addWidget(btnClear);
    layout->addLayout(ctrlLayout);

    // 缩放和旋转控制
    QHBoxLayout *transformLayout = new QHBoxLayout();
    transformLayout->addWidget(new QLabel("缩放："));
    m_scaleSlider = new QSlider(Qt::Horizontal);
    m_scaleSlider->setRange(20, 300);
    m_scaleSlider->setValue(100);
    transformLayout->addWidget(m_scaleSlider);
    QLabel *scaleLabel = new QLabel("100%");
    transformLayout->addWidget(scaleLabel);
    transformLayout->addSpacing(20);
    transformLayout->addWidget(new QLabel("旋转："));
    m_rotateSlider = new QSlider(Qt::Horizontal);
    m_rotateSlider->setRange(-180, 180);
    m_rotateSlider->setValue(0);
    transformLayout->addWidget(m_rotateSlider);
    QLabel *rotateLabel = new QLabel("0°");
    transformLayout->addWidget(rotateLabel);
    layout->addLayout(transformLayout);

    // 视图
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-300, -200, 600, 400);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setSceneRect(-300, -200, 600, 400);
    layout->addWidget(m_view, 1);

    // 信号连接
    connect(btnRect, &QPushButton::clicked, this, &BasicShapesTab::onAddRect);
    connect(btnEllipse, &QPushButton::clicked, this, &BasicShapesTab::onAddEllipse);
    connect(btnText, &QPushButton::clicked, this, &BasicShapesTab::onAddText);
    connect(btnLine, &QPushButton::clicked, this, &BasicShapesTab::onAddLine);
    connect(btnClear, &QPushButton::clicked, this, &BasicShapesTab::onClearScene);

    connect(m_scaleSlider, &QSlider::valueChanged, this, &BasicShapesTab::onScaleChanged);
    connect(m_rotateSlider, &QSlider::valueChanged, this, &BasicShapesTab::onRotateChanged);

    connect(m_scaleSlider, &QSlider::valueChanged, scaleLabel, [scaleLabel](int v) {
        scaleLabel->setText(QString("%1%").arg(v));
    });
    connect(m_rotateSlider, &QSlider::valueChanged, rotateLabel, [rotateLabel](int v) {
        rotateLabel->setText(QString("%1°").arg(v));
    });

    // 添加初始提示
    QGraphicsTextItem *hint = m_scene->addText("点击按钮添加图形，拖拽可移动\n使用滑块调整视图缩放和旋转");
    hint->setDefaultTextColor(QColor(150, 150, 150));
}

void BasicShapesTab::onAddRect()
{
    QGraphicsRectItem *rect = m_scene->addRect(
        -40, -30, 80, 60,
        QPen(Qt::darkBlue, 2), QBrush(QColor(100, 150, 255, 150)));
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable);
    rect->setPos(qrand() % 300 - 150, qrand() % 200 - 100);
}

void BasicShapesTab::onAddEllipse()
{
    QGraphicsEllipseItem *ellipse = m_scene->addEllipse(
        -35, -35, 70, 70,
        QPen(Qt::darkGreen, 2), QBrush(QColor(100, 255, 150, 150)));
    ellipse->setFlag(QGraphicsItem::ItemIsMovable);
    ellipse->setFlag(QGraphicsItem::ItemIsSelectable);
    ellipse->setPos(qrand() % 300 - 150, qrand() % 200 - 100);
}

void BasicShapesTab::onAddText()
{
    QGraphicsTextItem *text = new QGraphicsTextItem("Qt5 图形");
    text->setDefaultTextColor(Qt::darkRed);
    text->setFont(QFont("sans-serif", 14, QFont::Bold));
    text->setFlag(QGraphicsItem::ItemIsMovable);
    text->setFlag(QGraphicsItem::ItemIsSelectable);
    m_scene->addItem(text);
    text->setPos(qrand() % 300 - 150, qrand() % 200 - 100);
}

void BasicShapesTab::onAddLine()
{
    QGraphicsLineItem *line = m_scene->addLine(
        -50, 0, 50, 0, QPen(Qt::darkMagenta, 3, Qt::DashLine));
    line->setFlag(QGraphicsItem::ItemIsMovable);
    line->setFlag(QGraphicsItem::ItemIsSelectable);
    line->setPos(qrand() % 300 - 150, qrand() % 200 - 100);
}

void BasicShapesTab::onClearScene()
{
    m_scene->clear();
    QGraphicsTextItem *hint = m_scene->addText("场景已清空");
    hint->setDefaultTextColor(QColor(150, 150, 150));
}

void BasicShapesTab::onScaleChanged(int value)
{
    qreal s = value / 100.0;
    m_view->scale(s / m_view->transform().m11(), s / m_view->transform().m22());
}

void BasicShapesTab::onRotateChanged(int value)
{
    m_view->rotate(value - m_lastRotation);
    m_lastRotation = value;
}

// ============================================================================
// Tab2: 交互与选中
// ============================================================================

// 自定义矩形项 - 支持悬停效果和右键菜单
class InteractiveRectItem : public QGraphicsRectItem
{
public:
    InteractiveRectItem(const QRectF &rect, const QColor &color)
        : QGraphicsRectItem(rect)
        , m_color(color)
        , m_hovered(false)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        setCursor(Qt::OpenHandCursor);
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override
    {
        Q_UNUSED(event)
        m_hovered = true;
        update();
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override
    {
        Q_UNUSED(event)
        m_hovered = false;
        update();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        painter->setRenderHint(QPainter::Antialiasing);

        QPen pen(Qt::black, m_hovered ? 3 : 2);
        if (isSelected()) {
            pen.setColor(QColor(255, 100, 100));
            pen.setWidth(3);
        }
        painter->setPen(pen);

        QBrush brush(m_color);
        if (m_hovered) {
            brush = m_color.lighter(130);
        }
        painter->setBrush(brush);
        painter->drawRoundedRect(rect(), 5, 5);
    }

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override
    {
        QMenu menu;
        menu.addAction("修改颜色", [this]() {
            QColor newColor = QColor::fromHsv(qrand() % 360, 150, 200);
            m_color = newColor;
            update();
        });
        menu.addAction("删除", [this]() {
            scene()->removeItem(this);
            delete this;
        });
        menu.addSeparator();
        menu.addAction("置于顶层", [this]() {
            setZValue(100);
        });
        menu.addAction("置于底层", [this]() {
            setZValue(-100);
        });
        menu.exec(event->screenPos());
    }

private:
    QColor m_color;
    bool m_hovered;
};

InteractionTab::InteractionTab(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // 视图
    QVBoxLayout *viewLayout = new QVBoxLayout();
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-250, -200, 500, 400);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setContextMenuPolicy(Qt::DefaultContextMenu);
    m_view->setSceneRect(-250, -200, 500, 400);
    viewLayout->addWidget(m_view);

    QLabel *infoLabel = new QLabel("提示：拖拽移动 | 点击选中 | Ctrl+点击多选 | 框选多个 | 右键菜单");
    infoLabel->setWordWrap(true);
    viewLayout->addWidget(infoLabel);
    layout->addLayout(viewLayout, 2);

    // 信息面板
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->addWidget(new QLabel("选中信息："));
    m_infoDisplay = new QTextEdit();
    m_infoDisplay->setReadOnly(true);
    m_infoDisplay->setFont(QFont("Courier", 9));
    m_infoDisplay->setMaximumWidth(250);
    infoLayout->addWidget(m_infoDisplay);

    QPushButton *btnAddItems = new QPushButton("添加示例图形");
    infoLayout->addWidget(btnAddItems);
    layout->addLayout(infoLayout, 1);

    // 添加初始图形
    QList<QColor> colors = QList<QColor>()
        << QColor(255, 150, 150) << QColor(150, 255, 150) << QColor(150, 150, 255)
        << QColor(255, 255, 150) << QColor(255, 150, 255) << QColor(150, 255, 255);

    for (int i = 0; i < 6; ++i) {
        InteractiveRectItem *item = new InteractiveRectItem(
            QRectF(-40, -30, 80, 60), colors[i]);
        item->setPos(-150 + (i % 3) * 120, -80 + (i / 3) * 120);
        m_scene->addItem(item);
    }

    // 监听选择变化
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this]() {
        QList<QGraphicsItem *> selected = m_scene->selectedItems();
        QString info;
        info += QString("选中 %1 个项目：\n").arg(selected.size());
        foreach (QGraphicsItem *item, selected) {
            info += QString("  位置: (%.0f, %.0f)\n")
                .arg(item->pos().x()).arg(item->pos().y());
        }
        m_infoDisplay->setPlainText(info);
    });

    connect(btnAddItems, &QPushButton::clicked, this, [this]() {
        QColor color = QColor::fromHsv(qrand() % 360, 150, 200);
        InteractiveRectItem *item = new InteractiveRectItem(
            QRectF(-40, -30, 80, 60), color);
        item->setPos(qrand() % 300 - 150, qrand() % 200 - 100);
        m_scene->addItem(item);
    });
}

// ============================================================================
// Tab3: 碰撞检测
// ============================================================================

// 碰撞检测用矩形 - 带碰撞响应
class CollisionRectItem : public QGraphicsRectItem
{
public:
    CollisionRectItem(const QRectF &rect, const QColor &color, const QString &label)
        : QGraphicsRectItem(rect)
        , m_baseColor(color)
        , m_label(label)
        , m_colliding(false)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setAcceptHoverEvents(true);
        setCursor(Qt::SizeAllCursor);
    }

    void checkCollision(QGraphicsScene *scene, QTextEdit *display)
    {
        bool wasColliding = m_colliding;
        m_colliding = false;
        m_collidingItems.clear();

        // 检测与所有其他项的碰撞
        foreach (QGraphicsItem *other, scene->items()) {
            if (other == this) continue;
            if (this->collidesWithItem(other)) {
                m_colliding = true;
                m_collidingItems.append(other);
            }
        }

        if (m_colliding != wasColliding) {
            update();
            QString info;
            info += QString("[%1] ").arg(m_label);
            if (m_colliding) {
                info += QString("碰撞！与 %1 个项目重叠").arg(m_collidingItems.size());
            } else {
                info += "无碰撞";
            }
            if (display) {
                display->append(info);
            }
        }
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        painter->setRenderHint(QPainter::Antialiasing);

        QColor color = m_colliding ? QColor(255, 60, 60) : m_baseColor;
        QPen pen(m_colliding ? Qt::red : Qt::black, m_colliding ? 3 : 2);
        painter->setPen(pen);
        painter->setBrush(color);
        painter->drawRoundedRect(rect(), 5, 5);

        // 标签
        painter->setPen(Qt::white);
        painter->setFont(QFont("sans-serif", 9, QFont::Bold));
        painter->drawText(rect(), Qt::AlignCenter, m_label);
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override
    {
        if (change == ItemPositionHasChanged && scene()) {
            // 通知其他项更新碰撞状态
            foreach (QGraphicsItem *item, scene()->items()) {
                if (item != this && item->type() == QGraphicsRectItem::Type) {
                    // 强制重绘
                    item->update();
                }
            }
        }
        return QGraphicsRectItem::itemChange(change, value);
    }

private:
    QColor m_baseColor;
    QString m_label;
    bool m_colliding;
    QList<QGraphicsItem *> m_collidingItems;
};

CollisionTab::CollisionTab(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QVBoxLayout *viewLayout = new QVBoxLayout();
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-250, -200, 500, 400);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(-250, -200, 500, 400);
    viewLayout->addWidget(m_view);

    QLabel *infoLabel = new QLabel("提示：拖拽红色矩形，观察碰撞检测结果\n碰撞时矩形变为红色高亮");
    infoLabel->setWordWrap(true);
    viewLayout->addWidget(infoLabel);
    layout->addLayout(viewLayout, 2);

    // 信息面板
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->addWidget(new QLabel("碰撞日志："));
    m_infoDisplay = new QTextEdit();
    m_infoDisplay->setReadOnly(true);
    m_infoDisplay->setFont(QFont("Courier", 9));
    m_infoDisplay->setMaximumWidth(250);
    infoLayout->addWidget(m_infoDisplay);
    layout->addLayout(infoLayout, 1);

    // 添加静态障碍物
    QColor obsColor(100, 150, 200);
    QStringList labels;
    labels << "障碍A" << "障碍B" << "障碍C" << "障碍D";

    int positions[4][2] = {{-100, -60}, {60, -40}, {-80, 80}, {100, 60}};
    int sizes[4][2] = {{60, 40}, {50, 80}, {80, 50}, {70, 60}};

    for (int i = 0; i < 4; ++i) {
        CollisionRectItem *item = new CollisionRectItem(
            QRectF(0, 0, sizes[i][0], sizes[i][1]),
            QColor::fromHsv(i * 80, 120, 200),
            labels[i]);
        item->setPos(positions[i][0], positions[i][1]);
        m_scene->addItem(item);
    }

    // 添加可移动的碰撞检测对象
    CollisionRectItem *mover = new CollisionRectItem(
        QRectF(0, 0, 50, 50), QColor(200, 100, 100), "移动体");
    mover->setPos(0, 0);
    m_scene->addItem(mover);

    // 场景变化时检测碰撞
    connect(m_scene, &QGraphicsScene::changed, this, &CollisionTab::onSceneChanged);
}

void CollisionTab::onSceneChanged()
{
    // 定期检测碰撞
    QString info;
    foreach (QGraphicsItem *item, m_scene->items()) {
        if (item->type() == QGraphicsRectItem::Type) {
            // 碰撞检测会在 item 的 paint 中自动处理颜色变化
        }
    }
}

void CollisionTab::onMoveCollisionItem()
{
    // 由 scene changed 信号自动处理
}

// ============================================================================
// Tab4: 坐标变换
// ============================================================================

// 可变换的图形项
class TransformableRectItem : public QGraphicsRectItem
{
public:
    TransformableRectItem(const QRectF &rect, const QColor &color, const QString &label)
        : QGraphicsRectItem(rect)
        , m_color(color)
        , m_label(label)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setAcceptHoverEvents(true);
    }

    void setMyRotation(qreal angle) { setRotation(angle); }
    void setMyScale(qreal s) { setScale(s); }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        painter->setRenderHint(QPainter::Antialiasing);

        QPen pen(isSelected() ? Qt::blue : Qt::black, 2);
        painter->setPen(pen);
        painter->setBrush(m_color);
        painter->drawRoundedRect(rect(), 3, 3);

        // 画坐标轴指示
        painter->setPen(QPen(Qt::red, 2));
        painter->drawLine(0, 0, 30, 0);
        painter->setPen(QPen(Qt::green, 2));
        painter->drawLine(0, 0, 0, -30);

        painter->setPen(Qt::white);
        painter->setFont(QFont("sans-serif", 8));
        painter->drawText(rect(), Qt::AlignCenter, m_label);
    }

private:
    QColor m_color;
    QString m_label;
};

CoordinateTab::CoordinateTab(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QVBoxLayout *viewLayout = new QVBoxLayout();
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-300, -250, 600, 500);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(-300, -250, 600, 500);

    // 绘制坐标网格
    QPen gridPen(QColor(220, 220, 220), 0.5);
    for (int x = -300; x <= 300; x += 50) {
        m_scene->addLine(x, -250, x, 250, gridPen);
    }
    for (int y = -250; y <= 250; y += 50) {
        m_scene->addLine(-300, y, 300, y, gridPen);
    }
    // 坐标轴
    QPen axisPen(Qt::gray, 1);
    m_scene->addLine(-300, 0, 300, 0, axisPen);
    m_scene->addLine(0, -250, 0, 250, axisPen);

    viewLayout->addWidget(m_view);
    layout->addLayout(viewLayout, 2);

    // 控制面板
    QVBoxLayout *ctrlLayout = new QVBoxLayout();

    ctrlLayout->addWidget(new QLabel("旋转角度："));
    QSlider *rotSlider = new QSlider(Qt::Horizontal);
    rotSlider->setRange(-180, 180);
    rotSlider->setValue(0);
    ctrlLayout->addWidget(rotSlider);

    ctrlLayout->addWidget(new QLabel("缩放比例："));
    QSlider *scaleSlider = new QSlider(Qt::Horizontal);
    scaleSlider->setRange(20, 300);
    scaleSlider->setValue(100);
    ctrlLayout->addWidget(scaleSlider);

    QPushButton *btnInfo = new QPushButton("显示坐标信息");
    ctrlLayout->addWidget(btnInfo);

    ctrlLayout->addWidget(new QLabel("坐标转换结果："));
    m_infoDisplay = new QTextEdit();
    m_infoDisplay->setReadOnly(true);
    m_infoDisplay->setFont(QFont("Courier", 9));
    ctrlLayout->addWidget(m_infoDisplay);

    layout->addLayout(ctrlLayout, 1);

    // 添加可变换图形
    TransformableRectItem *rect1 = new TransformableRectItem(
        QRectF(-40, -30, 80, 60), QColor(100, 180, 255, 180), "项目A");
    rect1->setPos(-80, -50);
    m_scene->addItem(rect1);

    TransformableRectItem *rect2 = new TransformableRectItem(
        QRectF(-40, -30, 80, 60), QColor(255, 150, 100, 180), "项目B");
    rect2->setPos(80, 50);
    m_scene->addItem(rect2);

    TransformableRectItem *target = rect1;

    connect(rotSlider, &QSlider::valueChanged, this, [target, this](int v) {
        target->setMyRotation(v);
        onShowCoordInfo();
    });

    connect(scaleSlider, &QSlider::valueChanged, this, [target, this](int v) {
        target->setMyScale(v / 100.0);
        onShowCoordInfo();
    });

    connect(btnInfo, &QPushButton::clicked, this, &CoordinateTab::onShowCoordInfo);

    // 点击选择目标
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this]() {
        QList<QGraphicsItem *> items = m_scene->selectedItems();
        if (!items.isEmpty()) {
            QGraphicsItem *item = items.first();
            if (item->type() == QGraphicsRectItem::Type) {
                onShowCoordInfo();
            }
        }
    });
}

void CoordinateTab::onRotationChanged(int value)
{
    // 由 lambda 中处理实际逻辑
    Q_UNUSED(value)
}

void CoordinateTab::onScaleChanged(int value)
{
    // 由 lambda 中处理实际逻辑
    Q_UNUSED(value)
}

void CoordinateTab::onShowCoordInfo()
{
    QList<QGraphicsItem *> selected = m_scene->selectedItems();
    QGraphicsItem *item = selected.isEmpty() ? nullptr : selected.first();

    QString info;
    info += "===== 坐标变换信息 =====\n\n";

    if (item) {
        info += QString("项目位置（场景坐标）：(%.1f, %.1f)\n")
            .arg(item->pos().x()).arg(item->pos().y());
        info += QString("旋转角度：%.1f°\n").arg(item->rotation());
        info += QString("缩放比例：%.2f\n").arg(item->scale());

        info += QString("\n变换矩阵：\n");
        QTransform t = item->transform();
        info += QString("  m11=%.2f, m12=%.2f\n").arg(t.m11()).arg(t.m12());
        info += QString("  m21=%.2f, m22=%.2f\n").arg(t.m21()).arg(t.m22());
        info += QString("  dx=%.1f, dy=%.1f\n").arg(t.dx()).arg(t.dy());

        // 坐标转换演示
        QPointF scenePos(100, 50);
        QPointF itemPos = item->mapFromScene(scenePos);
        QPointF viewPos = m_view->mapFromScene(scenePos);

        info += QString("\n坐标转换示例（场景点 %.0f, %.0f）：\n")
            .arg(scenePos.x()).arg(scenePos.y());
        info += QString("  -> 项目坐标：(%.1f, %.1f)\n").arg(itemPos.x()).arg(itemPos.y());
        info += QString("  -> 视图坐标：(%.1f, %.1f)\n").arg(viewPos.x()).arg(viewPos.y());
    } else {
        info += "请点击选中一个图形项来查看坐标信息。\n";
    }

    info += "\n===== 坐标系说明 =====\n";
    info += "场景坐标：QGraphicsScene 的全局坐标系\n";
    info += "视图坐标：QGraphicsView 的像素坐标系\n";
    info += "项目坐标：QGraphicsItem 自身的局部坐标系\n";
    info += "mapToScene / mapFromScene：场景 <-> 项目转换\n";
    info += "mapToView / mapFromView：视图 <-> 场景转换";

    m_infoDisplay->setPlainText(info);
}

// ============================================================================
// Tab5: 自定义 Item
// ============================================================================

CustomItemTab::CustomItemTab(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QVBoxLayout *viewLayout = new QVBoxLayout();
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-350, -250, 700, 500);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::NoDrag);
    m_view->setSceneRect(-350, -250, 700, 500);

    // 绘制背景网格
    QPen gridPen(QColor(235, 235, 240), 0.5);
    for (int x = -350; x <= 350; x += 25) {
        m_scene->addLine(x, -250, x, 250, gridPen);
    }
    for (int y = -250; y <= 250; y += 25) {
        m_scene->addLine(-350, y, 350, y, gridPen);
    }

    viewLayout->addWidget(m_view);
    layout->addLayout(viewLayout, 2);

    // 控制面板
    QVBoxLayout *ctrlLayout = new QVBoxLayout();
    ctrlLayout->addWidget(new QLabel("流程图编辑器"));

    QPushButton *btnAddNode = new QPushButton("添加节点");
    ctrlLayout->addWidget(btnAddNode);

    QPushButton *btnAddConn = new QPushButton("连接选中节点");
    ctrlLayout->addWidget(btnAddConn);

    QPushButton *btnAutoConnect = new QPushButton("自动连线演示");
    ctrlLayout->addWidget(btnAutoConnect);

    QPushButton *btnClearAll = new QPushButton("清空全部");
    ctrlLayout->addWidget(btnClearAll);

    ctrlLayout->addWidget(new QLabel("操作日志："));
    m_infoDisplay = new QTextEdit();
    m_infoDisplay->setReadOnly(true);
    m_infoDisplay->setFont(QFont("Courier", 9));
    ctrlLayout->addWidget(m_infoDisplay);

    layout->addLayout(ctrlLayout, 1);

    // 信号连接
    connect(btnAddNode, &QPushButton::clicked, this, &CustomItemTab::onAddNode);
    connect(btnAddConn, &QPushButton::clicked, this, &CustomItemTab::onAddConnection);
    connect(btnAutoConnect, &QPushButton::clicked, this, &CustomItemTab::onAutoConnect);
    connect(btnClearAll, &QPushButton::clicked, this, &CustomItemTab::onClearAll);

    m_infoDisplay->append("[系统] 流程图编辑器就绪。");
    m_infoDisplay->append("[提示] 添加节点后可拖拽移动。");
    m_infoDisplay->append("[提示] 选中两个节点后点击连接。");
}

void CustomItemTab::onAddNode()
{
    static int nodeCounter = 0;
    ++nodeCounter;

    QStringList titles;
    titles << "开始" << "处理" << "判断" << "计算" << "输出" << "结束" << "输入" << "验证";
    QString title = nodeCounter <= titles.size()
        ? titles[nodeCounter - 1]
        : QString("节点%1").arg(nodeCounter);

    FlowNodeItem *node = new FlowNodeItem(title);
    node->setPos(-200 + (nodeCounter % 4) * 130, -150 + (nodeCounter / 4) * 100);
    m_scene->addItem(node);
    m_nodes.append(node);

    m_infoDisplay->append(QString("[添加] %1 (%2 个节点)")
        .arg(title).arg(m_nodes.size()));
}

void CustomItemTab::onAddConnection()
{
    QList<QGraphicsItem *> selected = m_scene->selectedItems();

    // 过滤出 FlowNodeItem
    QList<FlowNodeItem *> selectedNodes;
    foreach (QGraphicsItem *item, selected) {
        if (item->type() == FlowNodeItem::Type) {
            selectedNodes.append(static_cast<FlowNodeItem *>(item));
        }
    }

    if (selectedNodes.size() < 2) {
        m_infoDisplay->append("[警告] 请先选中两个节点（Ctrl+点击多选）！");
        return;
    }

    FlowNodeItem *from = selectedNodes.at(0);
    FlowNodeItem *to = selectedNodes.at(1);

    ConnectionLineItem *line = new ConnectionLineItem(from, to);
    m_scene->addItem(line);

    m_infoDisplay->append(QString("[连接] %1 -> %2").arg(from->m_title, to->m_title));
}

void CustomItemTab::onAutoConnect()
{
    if (m_nodes.size() < 2) {
        m_infoDisplay->append("[警告] 至少需要两个节点才能自动连线！");
        return;
    }

    m_infoDisplay->append("[自动连线] 开始...");

    for (int i = 0; i < m_nodes.size() - 1; ++i) {
        FlowNodeItem *from = m_nodes[i];
        FlowNodeItem *to = m_nodes[i + 1];

        ConnectionLineItem *line = new ConnectionLineItem(from, to);
        m_scene->addItem(line);
        m_infoDisplay->append(QString("  %1 -> %2").arg(from->m_title, to->m_title));
    }

    m_infoDisplay->append("[自动连线] 完成！");
}

void CustomItemTab::onClearAll()
{
    m_scene->clear();
    m_nodes.clear();

    // 重新绘制背景网格
    QPen gridPen(QColor(235, 235, 240), 0.5);
    for (int x = -350; x <= 350; x += 25) {
        m_scene->addLine(x, -250, x, 250, gridPen);
    }
    for (int y = -250; y <= 250; y += 25) {
        m_scene->addLine(-350, y, 350, y, gridPen);
    }

    m_infoDisplay->append("[系统] 已清空所有节点和连接。");
}

// ============================================================================
// MainWindow 主窗口
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Demo 20 - Graphics View 图形框架");
    resize(950, 700);

    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    m_tabWidget->addTab(new BasicShapesTab(), "基本图形");
    m_tabWidget->addTab(new InteractionTab(), "交互与选中");
    m_tabWidget->addTab(new CollisionTab(), "碰撞检测");
    m_tabWidget->addTab(new CoordinateTab(), "坐标变换");
    m_tabWidget->addTab(new CustomItemTab(), "自定义 Item");
}
