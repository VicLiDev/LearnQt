#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Demo 18 - JSON/XML 数据处理");
    window.resize(900, 700);
    window.show();

    return app.exec();
}
