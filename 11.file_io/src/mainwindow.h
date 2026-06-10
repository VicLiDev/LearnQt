#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QTextEdit;
class QAction;
class QMenu;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();

    void onDocumentModified();

private:
    Ui::MainWindow *ui;

    QString m_currentFilePath;

    static const int MAX_RECENT_FILES = 5;

    void loadFile(const QString &filePath);
    bool saveFile(const QString &filePath);
    bool maybeSave();

    void updateRecentFilesMenu();
    void addRecentFile(const QString &filePath);
    void clearRecentFiles();

    void updateStatusBar();
    void readSettings();
    void writeSettings();
};

#endif // MAINWINDOW_H
