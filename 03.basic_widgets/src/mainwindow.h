#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QLabel>
#include <QMainWindow>
#include <QTime>

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
    void onLineEditChanged(const QString &text);
    void onSliderValueChanged(int value);
    void onSpinBoxValueChanged(int value);
    void onDoubleSpinBoxValueChanged(double value);
    void onComboBoxIndexChanged(int index);
    void onComboBoxEditTextChanged(const QString &text);
    void onDateChanged(const QDate &date);
    void onTimeChanged(const QTime &time);

private:
    Ui::MainWindow *m_ui;
    QLabel *m_statusLabel;

    void setupWidgets();
};

#endif // MAINWINDOW_H
