#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QGraphicsObject>
#include <QGraphicsItem>

class QGraphicsScene;
class QGraphicsView;
class QTextEdit;
class QPushButton;
class QSlider;
class QLabel;

/* ========== 自定义图形项：可拖拽流程图节点 ========== */
class FlowNodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    enum { Type = UserType + 1 };
    explicit FlowNodeItem(const QString &title, QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString m_title;  // 节点标题（公开，方便外部访问）

    QPointF getInputPortPos() const;
    QPointF getOutputPortPos() const;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QColor m_color;
    bool m_hovered;
    static const int PORT_RADIUS;
};

/* ========== 自定义图形项：连接线 ========== */
class ConnectionLineItem : public QGraphicsObject
{
    Q_OBJECT
public:
    enum { Type = UserType + 2 };
    ConnectionLineItem(FlowNodeItem *from, FlowNodeItem *to, QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void updatePath();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    FlowNodeItem *m_fromNode;
    FlowNodeItem *m_toNode;
    QPainterPath m_path;
};

/* ========== Tab1: 基本图形 ========== */
class BasicShapesTab : public QWidget
{
    Q_OBJECT
public:
    explicit BasicShapesTab(QWidget *parent = nullptr);

private slots:
    void onAddRect();
    void onAddEllipse();
    void onAddText();
    void onAddLine();
    void onClearScene();
    void onScaleChanged(int value);
    void onRotateChanged(int value);

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QSlider *m_scaleSlider;
    QSlider *m_rotateSlider;
    int m_lastRotation;
};

/* ========== Tab2: 交互与选中 ========== */
class InteractionTab : public QWidget
{
    Q_OBJECT
public:
    explicit InteractionTab(QWidget *parent = nullptr);

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTextEdit *m_infoDisplay;
};

/* ========== Tab3: 碰撞检测 ========== */
class CollisionTab : public QWidget
{
    Q_OBJECT
public:
    explicit CollisionTab(QWidget *parent = nullptr);

private slots:
    void onSceneChanged();
    void onMoveCollisionItem();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTextEdit *m_infoDisplay;
};

/* ========== Tab4: 坐标变换 ========== */
class CoordinateTab : public QWidget
{
    Q_OBJECT
public:
    explicit CoordinateTab(QWidget *parent = nullptr);

private slots:
    void onRotationChanged(int value);
    void onScaleChanged(int value);
    void onShowCoordInfo();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTextEdit *m_infoDisplay;
};

/* ========== Tab5: 自定义 Item ========== */
class CustomItemTab : public QWidget
{
    Q_OBJECT
public:
    explicit CustomItemTab(QWidget *parent = nullptr);

private slots:
    void onAddNode();
    void onAddConnection();
    void onAutoConnect();
    void onClearAll();

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTextEdit *m_infoDisplay;
    QList<FlowNodeItem *> m_nodes;
};

/* ========== 主窗口 ========== */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QTabWidget *m_tabWidget;
};

#endif // MAINWINDOW_H
