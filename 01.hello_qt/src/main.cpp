#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("Hello Qt Demo");
    w.resize(400, 150);
    w.show();

    return app.exec();
}
