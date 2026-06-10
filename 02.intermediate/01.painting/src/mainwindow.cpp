#include "mainwindow.h"
#include "paintwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Demo 07 - Painting"));
    resize(700, 600);

    PaintWidget *paintWidget = new PaintWidget(this);
    setCentralWidget(paintWidget);
}
