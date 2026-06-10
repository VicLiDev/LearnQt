#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGraphicsOpacityEffect>
#include <QStateMachine>
#include <QState>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

class AnimationButton : public QPushButton
{
    Q_OBJECT
    // 用于 QPropertyAnimation 操作透明度
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit AnimationButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
    }

    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal val) { m_opacity = val; style()->unpolish(this); style()->polish(this); update(); }

private:
    qreal m_opacity;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();

    // === Tab 1: 属性动画 ===
    void createPropertyAnimationTab(QWidget *parent);

    // === Tab 2: 动画组 ===
    void createAnimationGroupTab(QWidget *parent);

    // === Tab 3: 窗口动画 ===
    void createWindowAnimationTab(QWidget *parent);

    // === Tab 4: 状态机 ===
    void createStateMachineTab(QWidget *parent);

    // === Tab 5: 综合示例 - 红绿灯 ===
    void createTrafficLightTab(QWidget *parent);

    // 辅助方法
    QPropertyAnimation *createMoveAnimation(QWidget *widget, const QPoint &start, const QPoint &end);
    void checkGroupFinished();

    // Tab1 控件
    QPushButton *m_propBtn;
    QComboBox   *m_easingCombo;
    QComboBox   *m_directionCombo;
    QComboBox   *m_loopCombo;
    QComboBox   *m_animTargetCombo;
    QPushButton *m_propStartBtn;
    QPushButton *m_propStopBtn;
    QPropertyAnimation *m_propAnim;

    // Tab2 控件
    QPushButton *m_seqBtn;
    QPushButton *m_parallelBtn;
    QPushButton *m_groupStartBtn;
    QPushButton *m_groupPauseBtn;
    QPushButton *m_groupResumeBtn;
    QPushButton *m_groupStopBtn;
    QLabel       *m_groupStatusLabel;
    QSequentialAnimationGroup *m_seqGroup;
    QParallelAnimationGroup   *m_paraGroup;

    // Tab3 控件
    QWidget *m_windowAnimWidget;
    QGraphicsOpacityEffect *m_opacityEffect;
    QPushButton *m_fadeInBtn;
    QPushButton *m_fadeOutBtn;
    QPushButton *m_moveBtn;
    QPushButton *m_scaleBtn;
    QPropertyAnimation *m_windowFadeAnim;
    QPropertyAnimation *m_windowMoveAnim;
    QPropertyAnimation *m_windowScaleAnim;

    // Tab4 控件
    QLabel       *m_stateLabel;
    QPushButton   *m_stateBtn;
    QStateMachine *m_stateMachine;

    // Tab5 控件 - 红绿灯
    QWidget       *m_redLight;
    QWidget       *m_yellowLight;
    QWidget       *m_greenLight;
    QPushButton   *m_trafficStartBtn;
    QPushButton   *m_trafficStopBtn;
    QLabel        *m_trafficStatusLabel;
    QStateMachine *m_trafficMachine;
};

#endif // MAINWINDOW_H
