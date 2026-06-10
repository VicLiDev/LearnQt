#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    layout = new QGridLayout(this);
    mWork = new DataBaseUserWork();
    mDBWidget =  new DataBaseWidget(this);
    setCentralWidget(mDBWidget);
    // layout->addWidget(mDBWidget);
    mDBWidget->show();
    // mDBWidget->setStyleSheet("background-color:rgb(255, 255, 0, 255);color:rgb(255, 0, 255, 255)");

}

MainWindow::~MainWindow()
{
    delete ui;
}

