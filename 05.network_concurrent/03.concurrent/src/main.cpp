#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Demo 22 - 并发编程");
    window.resize(800, 600);
    window.show();

    return app.exec();
}
