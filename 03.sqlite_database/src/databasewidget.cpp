#include "databasewidget.h"
#include "ui_databasewidget.h"

DataBaseWidget::DataBaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataBaseWidget)
{
    ui->setupUi(this);
    setWindowTitle("DataBase Widget");

    mTimer = new QTimer();
    layout = new QGridLayout(this);
    mSqlQueryMod = new QSqlQueryModel();
    view = new QTableView;
    mSqlQueryModFail = new QSqlQueryModel();
    viewFail = new QTableView;
    mSqlQueryModSum = new QSqlQueryModel();
    viewSum = new QTableView;
    mDB = QSqlDatabase::addDatabase("QSQLITE", "connectName_widget");


//    mDB.setHostName("localhost");
    mDB.setDatabaseName(FILEPATH+"test.db");  //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
//    mDB.setUserName("mdatabase");
//    mDB.setPassword("mdatabase");
    if(mDB.open()){
        qDebug() << "成功连接数据库：" << mDB.databaseName();
    }else{
        qDebug() << "数据库打开失败！";
    }

    layout->addWidget(view);
    layout->addWidget(viewFail);
    layout->addWidget(viewSum);

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setModel(mSqlQueryMod);
    view->setWindowTitle("view");
    view->show();

    viewFail->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewFail->setModel(mSqlQueryModFail);
    viewFail->show();

    viewSum->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewSum->setModel(mSqlQueryModSum);
    viewSum->show();

    connect(mTimer, SIGNAL(timeout()), this, SLOT(timerSolt()));
    mTimer->start(1000);  //以毫秒为单位

    refreshDisplay();
}

DataBaseWidget::~DataBaseWidget()
{
    delete ui;
}


void DataBaseWidget::refreshDisplay()
{
    if(mDB.tables().size() < 2)
        return;

    QString query_sql = QString("select * from %1").arg(mDB.tables().at(0));
    mSqlQueryMod->setQuery(query_sql, mDB);
    mSqlQueryMod->setHeaderData(0, Qt::Horizontal, "alter name");

    query_sql = QString("select * from %1 where id<5").arg(mDB.tables().at(0));
    mSqlQueryModFail->setQuery(query_sql, mDB);

    query_sql = QString("select * from %1").arg(mDB.tables().at(1));
    mSqlQueryModSum->setQuery(query_sql, mDB);

    return;
}

void DataBaseWidget::timerSolt()
{
    refreshDisplay();
}
