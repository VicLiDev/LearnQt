#ifndef DATABASEENCAP_H
#define DATABASEENCAP_H

#include<stdio.h>

#include<QVariant>
#include<QtSql/QSqlDatabase>
#include<QtSql/QSqlQuery>
#include<QSqlDriver>
#include<QSqlRecord>
#include<QSqlField>
#include<QSqlError>

#include<QDebug>
#include<QDateTime>
#include<QDate>

#define DATABASE_LOG_SW 0
#define DATABASE_LOG(str1, str2) \
do{ \
    if(DATABASE_LOG_SW){ \
        qDebug() << QString("DATABASE %1 >> (file: %2 func: %3 line: %4) %5").arg(str1, QString(__FILE__), \
                                                       QString(__func__), QString::number(__LINE__), str2);\
    } \
}while(0)

#define DATABASE_ENCAP_LOG(str) DATABASE_LOG(QString("ENCAP"), str)

#define FILEPATH QString("/home/administrator/test/")

enum DBType{
    INT,
    VARCHAR,
    DATE,
    TIME,
    DECIMAL,
};

struct ValTypePair{
    void *val;
    // enum DBType type;
};


class DataBaseEncap : public QObject
{
    Q_OBJECT
public:
    DataBaseEncap(QString connectName, QString dbName = "test.db", QObject *parent = nullptr);

    bool isTableExit(QString tableName);

    //-- table
    void createTable(QString tableName, QString createtable_sql);
    void dropTable(QString tableName);

    //-- date
    void insertDate(QString tableName, QString date_sql, struct ValTypePair pair[], int cnt);
    void deleteDate(QString tableName, QString date_sql);
    void updateDate(QString tableName, QString date_sql, int fieldIdx, QVariant val);
    void queryDate(QString tableName, QString date_sql, QSqlQuery &queryOut);

private:
    QSqlDatabase mDB;
    QSqlQuery mQuery;

    void dumpDBError();
    void dumpQueryError();

signals:
    void updateData();
};

#endif // DATABASEENCAP_H
