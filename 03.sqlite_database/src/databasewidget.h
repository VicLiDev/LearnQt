#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QTableView>
#include <QGridLayout>
#include <QTimer>

#include "databaseencap.h"

namespace Ui {
class DataBaseWidget;
}

class DataBaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataBaseWidget(QWidget *parent = nullptr);
    ~DataBaseWidget();

private:
    Ui::DataBaseWidget *ui;
    QSqlDatabase mDB;

    QGridLayout *layout;
    QSqlQueryModel *mSqlQueryMod;
    QTableView *view;
    QSqlQueryModel *mSqlQueryModFail;
    QTableView *viewFail;
    QSqlQueryModel *mSqlQueryModSum;
    QTableView *viewSum;
    QTimer *mTimer;

public slots:
    void timerSolt();
    void refreshDisplay();
};

#endif // DATABASEWIDGET_H
