#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("Qt Basic Widgets Demo");
    w.resize(480, 600);
    w.show();

    return app.exec();
}
