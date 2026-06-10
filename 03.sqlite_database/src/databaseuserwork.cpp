#include "databaseuserwork.h"

DataBaseUserWork::DataBaseUserWork(QObject *parent) :
    QObject(parent)
{
    mDBEnc = new DataBaseEncap("workDBCon", "test.db");

    mTimer = new QTimer(this);
    connect( mTimer, SIGNAL(timeout()), this, SLOT(timerSolt()) );
    mTimer->start(2000);  //以毫秒为单位

    this->moveToThread(&workeThd);
    workeThd.start();
}

void DataBaseUserWork::initTodayTable()
{
    if(QTime::currentTime().hour() >= TODAY_BEGIN_TIME){
        curDealTblDate = QDate::currentDate().toString(Qt::ISODate);
        toDropTblDate = QDate::currentDate().addDays(-1 * TABLRE_MAX_COUNT).toString(Qt::ISODate);
    }else{
        curDealTblDate = QDate::currentDate().addDays(-1).toString(Qt::ISODate);
        toDropTblDate = QDate::currentDate().addDays(-1).addDays(-1 * TABLRE_MAX_COUNT).toString(Qt::ISODate);
    }

    todayInfoTable = QString("lhj").append(curDealTblDate.remove('-')).append("info");
    todaySummaryTable = QString("lhj").append(curDealTblDate.remove('-')).append("summary");
    toDropInfoTable = QString("lhj").append(toDropTblDate.remove('-')).append("info");
    toDropSummaryTable = QString("lhj").append(toDropTblDate.remove('-')).append("summary");

    //-- create
    if(mDBEnc->isTableExit(todayInfoTable)){
        DATABASE_WORK_LOG(QString("table %1 is exits !!!").arg(todayInfoTable));
    }else{
        DATABASE_WORK_LOG(QString("table %1 isn't exits !!!").arg(todayInfoTable));
        // -- create info table
        QString create_sql = QString("CREATE TABLE %1 ("
                                     "ID INT ,"
                                     "Date Date,"
                                     "Time TIMESTAMP,"
                                     "DataType VARCHAR(20),"
                                     "Value1 FLOAT,"
                                     "Value2 FLOAT,"
                                     "Others VARCHAR(20))").arg(todayInfoTable);
        mDBEnc->createTable(todayInfoTable, create_sql);
        // mDBEnc->dropTable(todayInfoTable);
    }

    if(mDBEnc->isTableExit(todaySummaryTable)){
        DATABASE_WORK_LOG(QString("table %1 is exits !!!").arg(todaySummaryTable));
    }else{
        DATABASE_WORK_LOG(QString("table %1 isn't exits !!!").arg(todaySummaryTable));
        // -- create summary table
        QString create_sql = QString("CREATE TABLE %1 ("
                                     "Date Date,"
                                     "type VARCHAR(20),"
                                     "Count INT,"
                                     "Others VARCHAR(20))").arg(todaySummaryTable);
        mDBEnc->createTable(todaySummaryTable, create_sql);
    }

    //-- drop
    if(mDBEnc->isTableExit(toDropInfoTable)){
        DATABASE_WORK_LOG(QString("table %1 is exits !!!").arg(todaySummaryTable));
        mDBEnc->dropTable(toDropInfoTable);
    }
    if(mDBEnc->isTableExit(toDropSummaryTable)){
        DATABASE_WORK_LOG(QString("table %1 is exits !!!").arg(todaySummaryTable));
        mDBEnc->dropTable(toDropSummaryTable);
    }

}

void DataBaseUserWork::timerSolt()
{
    struct ValTypePair infoTbl[7];
    struct ValTypePair summaryTbl[4];
    static int id;
    QString curdate = QDate::currentDate().toString("yyyy-MM-dd");
    QString curtime = QTime::currentTime().toString("hh:mm:ss");
    QString type = QString("type").append(QString::number(id/3));
    double val1 = 2.11;
    double val2 = 2.22;
    QString others = "balabala";

    //======== table deal ========
    initTodayTable();

    //======== todayInfoTable ========
    infoTbl[0].val = &id;
    infoTbl[1].val = &curdate;
    infoTbl[2].val = &curtime;
    infoTbl[3].val = &type;
    infoTbl[4].val = &val1;
    infoTbl[5].val = &val2;
    infoTbl[6].val = &others;

    //-- insert
    QString insert_info_sql = QString("INSERT INTO %1 VALUES(:ID, :Date, :Time, :DataType, :Value1, :Value2, :Others) ").arg(todayInfoTable);
    mDBEnc->insertDate(todayInfoTable, insert_info_sql, infoTbl, 7);
    //-- delete
    QString delete_info_sql = QString("DELETE  FROM  %1  WHERE ID=3").arg(todayInfoTable);
    // mDBEnc->deleteDate(todayInfoTable, delete_info_sql);
    //-- query
    QString query_info_sql = QString("select * FROM %1 WHERE DataType='%2'").arg(todayInfoTable, type);
    QSqlQuery queryTmp;
    int msgCnt = 0;
    mDBEnc->queryDate(todayInfoTable, query_info_sql, queryTmp);
    while(queryTmp.next()){
#if 0
        qDebug() << queryTmp.value(0).toString() << " " << queryTmp.value(1).toString() << " " << queryTmp.value(2).toString()
                 << " " << queryTmp.value(3).toString() << " " << queryTmp.value(4).toString() << " " << queryTmp.value(5).toString()
                 << " " << queryTmp.value(6).toString();
#endif
        msgCnt++;
    }

    //======== todaySummaryTable ========
    if(msgCnt == 1){
        //-- insert
        summaryTbl[0].val = &curdate;
        summaryTbl[1].val = &type;
        summaryTbl[2].val = &msgCnt;
        summaryTbl[3].val = &others;
        QString insert_summary_sql = QString("INSERT INTO %1 VALUES(:Date, :type, :Count, :Others) ").arg(todaySummaryTable);
        mDBEnc->insertDate(todaySummaryTable, insert_summary_sql, summaryTbl, 4);
    }else{
        //-- update
        QString update_summary_sql = QString("UPDATE %1 SET Count='%2' WHERE type='%3'").arg(todaySummaryTable, QString::number(msgCnt), type);
        mDBEnc->updateDate(todaySummaryTable, update_summary_sql, 2, msgCnt);
    }


    id++;
    return;
}
