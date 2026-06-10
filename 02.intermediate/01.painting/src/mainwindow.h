#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PaintWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
};

#endif // MAINWINDOW_H
