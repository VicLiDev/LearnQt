#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QTextEdit;
class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onExit();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onAboutQt();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    Ui::MainWindow *ui;

    QTextEdit *m_textEdit;
    QLabel *m_statusLabel;

    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_exitAction;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_aboutQtAction;
};

#endif // MAINWINDOW_H
