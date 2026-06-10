#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>

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
    void onAddRow();
    void onRemoveRow();
    void onEditCell();

    void onAddListItem();
    void onRemoveListItem();

    void onAddChild();
    void onAddSibling();
    void onRemoveTreeItem();

private:
    Ui::MainWindow *ui;

    QStandardItemModel *m_tableModel;
    QStringListModel *m_listModel;
    QStandardItemModel *m_treeModel;

    void setupTableTab();
    void setupListTab();
    void setupTreeTab();
};

#endif // MAINWINDOW_H
