#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata for QSettings
    QApplication::setOrganizationName("LearnQt");
    QApplication::setApplicationName("Demo10_FileIO");

    MainWindow window;
    window.show();

    return app.exec();
}
