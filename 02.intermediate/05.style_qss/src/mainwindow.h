#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStyleChanged(const QString &styleName);
    void onLoadDarkTheme();
    void onLoadLightTheme();

private:
    Ui::MainWindow *ui;

    void populateStyleComboBox();
    void applyStyle(const QString &styleName);
};

#endif // MAINWINDOW_H
