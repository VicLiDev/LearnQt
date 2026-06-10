#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle(QObject::tr("24.自定义控件 - Qt5 学习 Demo"));
    window.resize(900, 700);
    window.show();

    return app.exec();
}
