#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Demo 20 - Graphics View 图形框架");
    window.resize(950, 700);
    window.show();

    return app.exec();
}
