#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class Counter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCountChanged(int value);
    void updateConnectionState();

private:
    Ui::MainWindow *m_ui;
    Counter *m_counter;

    QLabel *m_countLabel;
    QLabel *m_statusLabel;
    QPushButton *m_incrementButton;
    QPushButton *m_decrementButton;
    QPushButton *m_resetButton;
    QPushButton *m_connectButton;
    QPushButton *m_disconnectButton;

    bool m_connected;

    void doConnect();
    void doDisconnect();
    void updateCountDisplay();
};

#endif // MAINWINDOW_H
