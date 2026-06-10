#include "databaseencap.h"

DataBaseEncap::DataBaseEncap(QString connectName, QString dbName, QObject *parent) :
    QObject(parent)
{
    mDB = QSqlDatabase::addDatabase("QSQLITE", connectName);
//    mDB.setHostName("localhost");
    mDB.setDatabaseName(FILEPATH+dbName);  //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
//    mDB.setUserName("mdatabase");
//    mDB.setPassword("mdatabase");
    if(mDB.open()){
        mQuery = QSqlQuery(mDB);
        DATABASE_ENCAP_LOG(QString("DataBaseEncap open database %1 sucess !!!"
                                  "  database path: %2").arg(dbName, mDB.databaseName()));
    }else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap open database %1 failed !!!").arg(dbName));
    }
}

void DataBaseEncap::dumpDBError()
{
    QSqlError err;
    err = mDB.lastError();
    if(err.isValid())//发生错误时isValid()返回true
    {
        switch (err.type()) {
        case QSqlError::NoError:
            DATABASE_ENCAP_LOG("DataBaseEncap no error");
            break;
        case QSqlError::ConnectionError://连接错语
            DATABASE_ENCAP_LOG(err.text());
            break;
        case QSqlError::StatementError://语句错语
            DATABASE_ENCAP_LOG(err.text());
            break;
        case QSqlError::TransactionError://事务错误
            DATABASE_ENCAP_LOG(err.text());
            break;
        default://未知错误
            DATABASE_ENCAP_LOG(err.text());
            break;
        }
    }
    return;
}

void DataBaseEncap::dumpQueryError()
{
    QSqlError err;
    err = mQuery.lastError();
    if(err.isValid())//发生错误时isValid()返回true
    {
        switch (err.type()) {
        case QSqlError::NoError:
            DATABASE_ENCAP_LOG("DataBaseEncap no error");
            break;
        case QSqlError::ConnectionError://连接错语
            DATABASE_ENCAP_LOG(err.text());
            break;
        case QSqlError::StatementError://语句错语
            DATABASE_ENCAP_LOG(err.text());
            break;
        case QSqlError::TransactionError://事务错误
            DATABASE_ENCAP_LOG(err.text());
            break;
        default:                         //未知错误
            DATABASE_ENCAP_LOG(err.text());
            break;
        }
    }
    return;
}

bool DataBaseEncap::isTableExit(QString tableName)
{
    return mDB.tables().contains(tableName);
}

void DataBaseEncap::createTable(QString tableName,  QString createtable_sql)
{
    DATABASE_ENCAP_LOG(createtable_sql);
    mQuery.prepare(createtable_sql);
    if(mQuery.exec())
        DATABASE_ENCAP_LOG(QString("DataBaseEncap create table sucess: %1").arg(tableName));
    else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap create table fail: %1").arg(tableName));
        DATABASE_ENCAP_LOG(createtable_sql);
        dumpQueryError();
    }
}

void DataBaseEncap::dropTable(QString tableName)
{
    QString droptable_sql = "DROP TABLE " + tableName;
    mQuery.prepare(droptable_sql);
    if(mQuery.exec())
        DATABASE_ENCAP_LOG(QString("DataBaseEncap drop table sucess: %1").arg(tableName));
    else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap drop table failed: %1").arg(tableName));
        DATABASE_ENCAP_LOG(droptable_sql);
        dumpQueryError();
    }
}

void DataBaseEncap::insertDate(QString tableName, QString date_sql, struct ValTypePair pair[], int cnt)
{
    //-- 获取表的字段个数
    QSqlDriver *fa = mDB.driver();
    QSqlRecord tableR = fa->record(tableName);
    int fielcnt = tableR.count();
    if(cnt != fielcnt){
        DATABASE_ENCAP_LOG(QString("DataBaseEncap insert date cnt is too more! %1").arg(tableName));
        return;
    }

    //-- insert
    mQuery.prepare(date_sql);
    for(int i = 0; i < cnt; i++){
        if(tableR.field(i).type() == QVariant::Int){
            mQuery.bindValue(":"+tableR.fieldName(i), (*(int *)pair[i].val));
        }else if(tableR.field(i).type() == QVariant::String){
            mQuery.bindValue(":"+tableR.fieldName(i), *(QString *)pair[i].val);
        }else if(tableR.field(i).type() == QVariant::Double){
            mQuery.bindValue(":"+tableR.fieldName(i), (*(double *)pair[i].val));
        }
    }
    if(mQuery.exec())
        DATABASE_ENCAP_LOG(QString("DataBaseEncap insert date sucess: %1").arg(tableName));
    else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap insert date failed: %1").arg(tableName));
        DATABASE_ENCAP_LOG(date_sql);
        dumpQueryError();
    }

    emit updateData();

    return;
}

void DataBaseEncap::deleteDate(QString tableName, QString date_sql)
{
    mQuery.prepare(date_sql);
    if(mQuery.exec())
        DATABASE_ENCAP_LOG(QString("DataBaseEncap delete date sucess: %1").arg(tableName));
    else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap delete date failed: %1").arg(tableName));
        DATABASE_ENCAP_LOG(date_sql);
        dumpQueryError();
    }

    emit updateData();

    return;
}

void DataBaseEncap::updateDate(QString tableName, QString date_sql, int fieldIdx, QVariant val)
{
    //-- 获取表的字段个数
    QSqlDriver *fa = mDB.driver();
    QSqlRecord tableR = fa->record(tableName);

    //-- insert
    mQuery.prepare(date_sql);
    if(tableR.field(fieldIdx).type() == QVariant::Int){
        mQuery.bindValue(":"+tableR.fieldName(fieldIdx), val);
    }else if(tableR.field(fieldIdx).type() == QVariant::String){
        mQuery.bindValue(":"+tableR.fieldName(fieldIdx), val);
    }else if(tableR.field(fieldIdx).type() == QVariant::Double){
        mQuery.bindValue(":"+tableR.fieldName(fieldIdx), val);
    }

    mQuery.prepare(date_sql);
    if(mQuery.exec())
        DATABASE_ENCAP_LOG(QString("DataBaseEncap update date sucess: %1").arg(tableName));
    else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap update date failed: %1").arg(tableName));
        DATABASE_ENCAP_LOG(date_sql);
        dumpQueryError();
    }

    emit updateData();

    return;
}

void DataBaseEncap::queryDate(QString tableName, QString date_sql, QSqlQuery &queryOut)
{
    mQuery.prepare(date_sql);
    if(mQuery.exec()){
        DATABASE_ENCAP_LOG(QString("DataBaseEncap query date sucess: %1").arg(tableName));
        queryOut = mQuery;
    }else{
        DATABASE_ENCAP_LOG(QString("DataBaseEncap query date failed: %1").arg(tableName));
        DATABASE_ENCAP_LOG(date_sql);
        dumpQueryError();
    }

    emit updateData();

    return;
}
