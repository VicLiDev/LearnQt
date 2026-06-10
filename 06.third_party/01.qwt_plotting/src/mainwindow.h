#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void timerEvent( QTimerEvent * event );  //方法一:当达到startTimer函数指定的时间之后会进入这个函数,可以通过这个函数执行相关操作

private slots:
    void on_comboBox_activated(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();  //方法二 定时器

    void timer2solt();

private:
    Ui::MainWindow *ui;

    int my_timerID;   //方法一的定时器ID  使用QObject类提供的定时器
    QTimer *my_timer2;  //方法二的定时器ID  使用Qtimer对象
};

#endif // MAINWINDOW_H
