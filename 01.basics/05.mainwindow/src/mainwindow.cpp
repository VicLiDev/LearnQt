#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTextEdit>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_textEdit(nullptr)
    , m_statusLabel(nullptr)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_exitAction(nullptr)
    , m_undoAction(nullptr)
    , m_redoAction(nullptr)
    , m_cutAction(nullptr)
    , m_copyAction(nullptr)
    , m_pasteAction(nullptr)
    , m_aboutQtAction(nullptr)
{
    ui->setupUi(this);

    // Create central text editor
    m_textEdit = new QTextEdit(this);
    ui->centralWidget->layout()->addWidget(m_textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    // File actions
    m_newAction = new QAction(tr("&New"), this);
    m_newAction->setShortcuts(QKeySequence::New);
    m_newAction->setStatusTip(tr("Create a new file"));
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNew);

    m_openAction = new QAction(tr("&Open"), this);
    m_openAction->setShortcuts(QKeySequence::Open);
    m_openAction->setStatusTip(tr("Open an existing file"));
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpen);

    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcuts(QKeySequence::Save);
    m_saveAction->setStatusTip(tr("Save the current file"));
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSave);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcuts(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Exit the application"));
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExit);

    // Edit actions
    m_undoAction = new QAction(tr("&Undo"), this);
    m_undoAction->setShortcuts(QKeySequence::Undo);
    m_undoAction->setStatusTip(tr("Undo the last operation"));
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::onUndo);

    m_redoAction = new QAction(tr("&Redo"), this);
    m_redoAction->setShortcuts(QKeySequence::Redo);
    m_redoAction->setStatusTip(tr("Redo the last undone operation"));
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::onRedo);

    m_cutAction = new QAction(tr("Cu&t"), this);
    m_cutAction->setShortcuts(QKeySequence::Cut);
    m_cutAction->setStatusTip(tr("Cut the selected text"));
    connect(m_cutAction, &QAction::triggered, this, &MainWindow::onCut);

    m_copyAction = new QAction(tr("&Copy"), this);
    m_copyAction->setShortcuts(QKeySequence::Copy);
    m_copyAction->setStatusTip(tr("Copy the selected text"));
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::onCopy);

    m_pasteAction = new QAction(tr("&Paste"), this);
    m_pasteAction->setShortcuts(QKeySequence::Paste);
    m_pasteAction->setStatusTip(tr("Paste text from clipboard"));
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::onPaste);

    // Help actions
    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    m_aboutQtAction->setStatusTip(tr("Show the Qt library's About dialog"));
    connect(m_aboutQtAction, &QAction::triggered, this, &MainWindow::onAboutQt);
}

void MainWindow::createMenus()
{
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // Edit menu
    QMenu *editMenu = menuBar->addMenu(tr("&Edit"));
    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();
    editMenu->addAction(m_cutAction);
    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);

    // Help menu
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBars()
{
    QToolBar *toolBar = ui->mainToolBar;
    toolBar->addAction(m_newAction);
    toolBar->addAction(m_openAction);
    toolBar->addAction(m_saveAction);
    toolBar->addSeparator();
    toolBar->addAction(m_undoAction);
    toolBar->addAction(m_redoAction);
}

void MainWindow::createStatusBar()
{
    QStatusBar *statusBar = this->statusBar();
    m_statusLabel = new QLabel(tr("Ready"), this);
    statusBar->addPermanentWidget(m_statusLabel);
}

void MainWindow::onNew()
{
    m_textEdit->clear();
    statusBar()->showMessage(tr("Action: New triggered"), 2000);
    m_statusLabel->setText(tr("New"));
}

void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (!fileName.isEmpty()) {
        statusBar()->showMessage(tr("Opened: %1").arg(fileName), 2000);
        m_statusLabel->setText(tr("Opened"));
    }
}

void MainWindow::onSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (!fileName.isEmpty()) {
        statusBar()->showMessage(tr("Saved: %1").arg(fileName), 2000);
        m_statusLabel->setText(tr("Saved"));
    }
}

void MainWindow::onExit()
{
    statusBar()->showMessage(tr("Action: Exit triggered"), 2000);
    m_statusLabel->setText(tr("Exit"));
    close();
}

void MainWindow::onUndo()
{
    m_textEdit->undo();
    statusBar()->showMessage(tr("Action: Undo triggered"), 2000);
    m_statusLabel->setText(tr("Undo"));
}

void MainWindow::onRedo()
{
    m_textEdit->redo();
    statusBar()->showMessage(tr("Action: Redo triggered"), 2000);
    m_statusLabel->setText(tr("Redo"));
}

void MainWindow::onCut()
{
    m_textEdit->cut();
    statusBar()->showMessage(tr("Action: Cut triggered"), 2000);
    m_statusLabel->setText(tr("Cut"));
}

void MainWindow::onCopy()
{
    m_textEdit->copy();
    statusBar()->showMessage(tr("Action: Copy triggered"), 2000);
    m_statusLabel->setText(tr("Copy"));
}

void MainWindow::onPaste()
{
    m_textEdit->paste();
    statusBar()->showMessage(tr("Action: Paste triggered"), 2000);
    m_statusLabel->setText(tr("Paste"));
}

void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
    statusBar()->showMessage(tr("Action: About Qt triggered"), 2000);
    m_statusLabel->setText(tr("About Qt"));
}
