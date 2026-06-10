#ifndef DATABASEUSERWORK_H
#define DATABASEUSERWORK_H

#include <QThread>
#include <QDate>
#include <QTime>
#include <QTimer>

#include "databaseencap.h"

#define DATABASE_WORK_LOG(str) DATABASE_LOG(QString("WORK"), str)

#define TODAY_BEGIN_TIME 8  // 8 clock
#define TABLRE_MAX_COUNT (365*3) // 3 years

class DataBaseUserWork : public QObject
{
    Q_OBJECT
public:
    DataBaseUserWork(QObject *parent = nullptr);
    virtual ~DataBaseUserWork(){};

private:
    DataBaseEncap *mDBEnc;
    QString curDealTblDate;
    QString toDropTblDate;
    QString todayInfoTable;
    QString todaySummaryTable;
    QString toDropInfoTable;
    QString toDropSummaryTable;

    QThread workeThd;

    QTimer *mTimer;

private slots:
    void initTodayTable();
    void timerSolt();
};

#endif // DATABASEUSERWORK_H
