#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tableModel(nullptr)
    , m_listModel(nullptr)
    , m_treeModel(nullptr)
{
    ui->setupUi(this);

    setupTableTab();
    setupListTab();
    setupTreeTab();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTableTab()
{
    m_tableModel = new QStandardItemModel(this);
    m_tableModel->setColumnCount(3);
    m_tableModel->setHorizontalHeaderLabels({"Name", "Age", "Score"});

    struct RowData {
        const char *name;
        int age;
        int score;
    };

    RowData sampleData[] = {
        {"Alice",   22, 95},
        {"Bob",     25, 87},
        {"Charlie", 20, 92},
        {"Diana",   23, 98},
        {"Edward",  21, 85}
    };

    for (int i = 0; i < 5; ++i) {
        QList<QStandardItem *> rowItems;
        rowItems.append(new QStandardItem(sampleData[i].name));
        rowItems.append(new QStandardItem(QString::number(sampleData[i].age)));
        rowItems.append(new QStandardItem(QString::number(sampleData[i].score)));
        m_tableModel->appendRow(rowItems);
    }

    ui->tableView->setModel(m_tableModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->addRowBtn, &QPushButton::clicked, this, &MainWindow::onAddRow);
    connect(ui->removeRowBtn, &QPushButton::clicked, this, &MainWindow::onRemoveRow);
    connect(ui->editCellBtn, &QPushButton::clicked, this, &MainWindow::onEditCell);
}

void MainWindow::setupListTab()
{
    m_listModel = new QStringListModel(this);
    QStringList items;
    items << "Apple" << "Banana" << "Cherry" << "Date" << "Elderberry";
    m_listModel->setStringList(items);

    ui->listView->setModel(m_listModel);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->addListItemBtn, &QPushButton::clicked, this, &MainWindow::onAddListItem);
    connect(ui->removeListItemBtn, &QPushButton::clicked, this, &MainWindow::onRemoveListItem);
}

void MainWindow::setupTreeTab()
{
    m_treeModel = new QStandardItemModel(this);
    m_treeModel->setHorizontalHeaderLabels({"Item"});

    // Root items
    QStandardItem *rootFruits = new QStandardItem("Fruits");
    QStandardItem *rootVegetables = new QStandardItem("Vegetables");

    // Children of Fruits
    QStandardItem *fruitApple = new QStandardItem("Apple");
    QStandardItem *fruitBanana = new QStandardItem("Banana");

    // Grandchildren of Apple
    fruitApple->appendRow(new QStandardItem("Red Apple"));
    fruitApple->appendRow(new QStandardItem("Green Apple"));

    // Children of Banana
    fruitBanana->appendRow(new QStandardItem("Cavendish"));
    fruitBanana->appendRow(new QStandardItem("Plantain"));

    rootFruits->appendRow(fruitApple);
    rootFruits->appendRow(fruitBanana);

    // Children of Vegetables
    QStandardItem *vegCarrot = new QStandardItem("Carrot");
    QStandardItem *vegBroccoli = new QStandardItem("Broccoli");

    vegCarrot->appendRow(new QStandardItem("Baby Carrot"));
    vegBroccoli->appendRow(new QStandardItem("Chinese Broccoli"));

    rootVegetables->appendRow(vegCarrot);
    rootVegetables->appendRow(vegBroccoli);

    m_treeModel->appendRow(rootFruits);
    m_treeModel->appendRow(rootVegetables);

    ui->treeView->setModel(m_treeModel);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->header()->setStretchLastSection(true);

    // Expand root items by default
    ui->treeView->expandAll();

    connect(ui->addChildBtn, &QPushButton::clicked, this, &MainWindow::onAddChild);
    connect(ui->addSiblingBtn, &QPushButton::clicked, this, &MainWindow::onAddSibling);
    connect(ui->removeTreeItemBtn, &QPushButton::clicked, this, &MainWindow::onRemoveTreeItem);
}

// === Table View Slots ===

void MainWindow::onAddRow()
{
    QList<QStandardItem *> rowItems;
    rowItems.append(new QStandardItem("New Name"));
    rowItems.append(new QStandardItem("0"));
    rowItems.append(new QStandardItem("0"));
    m_tableModel->appendRow(rowItems);
}

void MainWindow::onRemoveRow()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Info", "Please select a row to remove.");
        return;
    }
    m_tableModel->removeRow(selected.first().row());
}

void MainWindow::onEditCell()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Info", "Please select a cell to edit.");
        return;
    }

    QModelIndex index = selected.first();
    QString currentValue = m_tableModel->data(index).toString();
    QString newValue = QInputDialog::getText(this, "Edit Cell", "New value:",
                                            QLineEdit::Normal, currentValue);
    if (!newValue.isEmpty()) {
        m_tableModel->setData(index, newValue);
    }
}

// === List View Slots ===

void MainWindow::onAddListItem()
{
    QString text = ui->listInput->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    QStringList list = m_listModel->stringList();
    list.append(text);
    m_listModel->setStringList(list);
    ui->listInput->clear();
}

void MainWindow::onRemoveListItem()
{
    QModelIndexList selected = ui->listView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Info", "Please select an item to remove.");
        return;
    }

    QStringList list = m_listModel->stringList();
    list.removeAt(selected.first().row());
    m_listModel->setStringList(list);
}

// === Tree View Slots ===

void MainWindow::onAddChild()
{
    QModelIndex current = ui->treeView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "Info", "Please select an item first.");
        return;
    }

    QString name = QInputDialog::getText(this, "Add Child", "Child name:");
    if (name.isEmpty()) {
        return;
    }

    QStandardItem *parentItem = m_treeModel->itemFromIndex(current);
    if (parentItem) {
        parentItem->appendRow(new QStandardItem(name));
    }
}

void MainWindow::onAddSibling()
{
    QModelIndex current = ui->treeView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "Info", "Please select an item first.");
        return;
    }

    QString name = QInputDialog::getText(this, "Add Sibling", "Sibling name:");
    if (name.isEmpty()) {
        return;
    }

    QStandardItem *item = m_treeModel->itemFromIndex(current);
    if (item) {
        QStandardItem *parent = item->parent();
        if (!parent) {
            parent = m_treeModel->invisibleRootItem();
        }
        parent->insertRow(item->row() + 1, new QStandardItem(name));
    }
}

void MainWindow::onRemoveTreeItem()
{
    QModelIndex current = ui->treeView->currentIndex();
    if (!current.isValid()) {
        QMessageBox::information(this, "Info", "Please select an item to remove.");
        return;
    }

    m_treeModel->removeRow(current.row(), current.parent());
}
