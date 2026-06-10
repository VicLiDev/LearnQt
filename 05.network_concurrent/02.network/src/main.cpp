#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Demo 19 - 网络编程");
    window.resize(900, 700);
    window.show();

    return app.exec();
}
