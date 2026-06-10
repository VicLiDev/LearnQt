#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;

// PaintArea: a custom widget that draws a circle whose size changes with the wheel
class PaintArea : public QWidget
{
    Q_OBJECT

public:
    explicit PaintArea(QWidget *parent = nullptr);

    int circleRadius() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int m_radius;
    static const int MIN_RADIUS = 20;
    static const int MAX_RADIUS = 200;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    PaintArea *m_paintArea;
    QLabel *m_mouseLabel;
    QLabel *m_keyLabel;
    QLabel *m_wheelLabel;
    QLabel *m_filterLabel;
};

#endif // MAINWINDOW_H
