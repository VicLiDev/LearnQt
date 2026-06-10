#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>
#include <QCloseEvent>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // File menu connections
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);

    // Edit menu connections - QTextEdit built-in slots
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QTextEdit::paste);

    // Track document modifications
    connect(ui->textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::onDocumentModified);

    // Set up recent files menu
    updateRecentFilesMenu();

    // Read settings
    readSettings();

    updateStatusBar();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onNewFile()
{
    if (maybeSave()) {
        ui->textEdit->clear();
        m_currentFilePath.clear();
        ui->textEdit->document()->setModified(false);
        setWindowTitle("Demo 10 - File I/O - Untitled");
        updateStatusBar();
    }
}

void MainWindow::onOpenFile()
{
    if (maybeSave()) {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Open File", QString(),
            "Text Files (*.txt);;All Files (*)");

        if (!filePath.isEmpty()) {
            loadFile(filePath);
            addRecentFile(filePath);
        }
    }
}

void MainWindow::onSaveFile()
{
    if (m_currentFilePath.isEmpty()) {
        onSaveFileAs();
    } else {
        saveFile(m_currentFilePath);
    }
}

void MainWindow::onSaveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(
        this, "Save As", QString(),
        "Text Files (*.txt);;All Files (*)");

    if (!filePath.isEmpty()) {
        if (saveFile(filePath)) {
            m_currentFilePath = filePath;
            addRecentFile(filePath);
            setWindowTitle(QString("Demo 10 - File I/O - %1").arg(QFileInfo(filePath).fileName()));
            updateStatusBar();
        }
    }
}

void MainWindow::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Open Error",
                             QString("Could not open file:\n%1\n%2")
                                 .arg(filePath)
                                 .arg(file.errorString()));
        return;
    }

    QByteArray data = file.readAll();
    ui->textEdit->setPlainText(QString::fromUtf8(data));

    file.close();

    m_currentFilePath = filePath;
    ui->textEdit->document()->setModified(false);
    setWindowTitle(QString("Demo 10 - File I/O - %1").arg(QFileInfo(filePath).fileName()));
    updateStatusBar();
}

bool MainWindow::saveFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Save Error",
                             QString("Could not save file:\n%1\n%2")
                                 .arg(filePath)
                                 .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();

    file.close();

    ui->textEdit->document()->setModified(false);
    updateStatusBar();
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified()) {
        return true;
    }

    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "Unsaved Changes",
        "The document has been modified.\nDo you want to save your changes?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        onSaveFile();
        return !ui->textEdit->document()->isModified();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }

    return true;
}

void MainWindow::onDocumentModified()
{
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    QString fileName;
    if (m_currentFilePath.isEmpty()) {
        fileName = "Untitled";
    } else {
        fileName = QFileInfo(m_currentFilePath).fileName();
    }

    QString modified = ui->textEdit->document()->isModified() ? " [Modified]" : "";
    statusBar()->showMessage(QString("File: %1%2").arg(fileName, modified));
}

void MainWindow::updateRecentFilesMenu()
{
    QMenu *recentMenu = ui->menuRecentFiles;
    if (!recentMenu) {
        return;
    }

    recentMenu->clear();

    QSettings settings;
    QStringList files = settings.value("recentFiles").toStringList();

    if (files.isEmpty()) {
        QAction *emptyAction = recentMenu->addAction("No Recent Files");
        emptyAction->setEnabled(false);
        return;
    }

    for (int i = 0; i < files.size(); ++i) {
        QAction *action = recentMenu->addAction(
            QString("%1. %2").arg(i + 1).arg(QFileInfo(files[i]).fileName()));

        connect(action, &QAction::triggered, this, [this, files, i]() {
            if (maybeSave()) {
                loadFile(files[i]);
            }
        });
    }

    recentMenu->addSeparator();
    QAction *clearAction = recentMenu->addAction("Clear Recent Files");
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
}

void MainWindow::addRecentFile(const QString &filePath)
{
    QSettings settings;
    QStringList files = settings.value("recentFiles").toStringList();

    files.removeAll(filePath);
    files.prepend(filePath);

    while (files.size() > MAX_RECENT_FILES) {
        files.removeLast();
    }

    settings.setValue("recentFiles", files);
    updateRecentFilesMenu();
}

void MainWindow::clearRecentFiles()
{
    QSettings settings;
    settings.remove("recentFiles");
    updateRecentFilesMenu();
}

void MainWindow::readSettings()
{
    QSettings settings;

    // Restore window geometry
    const QByteArray geometry = settings.value("geometry").toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    // Restore last opened file
    const QString lastFile = settings.value("lastFile").toString();
    if (!lastFile.isEmpty()) {
        QFile file(lastFile);
        if (file.exists()) {
            loadFile(lastFile);
        }
    }

    updateRecentFilesMenu();
}

void MainWindow::writeSettings()
{
    QSettings settings;

    // Save window geometry
    settings.setValue("geometry", saveGeometry());

    // Save current file
    if (!m_currentFilePath.isEmpty()) {
        settings.setValue("lastFile", m_currentFilePath);
    }
}
