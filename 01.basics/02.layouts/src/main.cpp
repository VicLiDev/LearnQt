#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("Qt Layouts Demo");
    w.resize(500, 400);
    w.show();

    return app.exec();
}
