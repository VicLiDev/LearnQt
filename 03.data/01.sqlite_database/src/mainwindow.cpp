#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mWork = new DataBaseUserWork();
    mDBWidget =  new DataBaseWidget(this);
    setCentralWidget(mDBWidget);
    mDBWidget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

