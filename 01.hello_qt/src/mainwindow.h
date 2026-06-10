#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onResetClicked();

private:
    QLabel *m_label;
    QPushButton *m_clickButton;
    QPushButton *m_resetButton;
    int m_counter;

    void updateLabel();
};

#endif // MAINWINDOW_H
