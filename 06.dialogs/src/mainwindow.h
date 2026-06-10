#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTextEdit;
class QLabel;

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
    void onOpenFile();
    void onSaveFile();
    void onSelectColor();
    void onGetText();
    void onGetInt();
    void onGetItem();
    void onQuestion();
    void onWarning();
    void onInformation();
    void onCritical();
    void onCustomDialog();

private:
    void createWidgets();
    void log(const QString &message);

    Ui::MainWindow *ui;

    QTextEdit *m_logEdit;
    QLabel *m_colorLabel;
};

#endif // MAINWINDOW_H
