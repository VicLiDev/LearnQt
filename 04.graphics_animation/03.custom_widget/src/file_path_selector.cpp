#include "file_path_selector.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QEvent>
#include <QFont>

// ============================================================
// FilePathSelector 构造函数
// ============================================================
FilePathSelector::FilePathSelector(QWidget *parent)
    : QWidget(parent)
    , mFilter("所有文件 (*);;文本文件 (*.txt)")
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // 路径输入框
    mLineEdit = new QLineEdit;
    mLineEdit->setPlaceholderText(tr("请输入或选择文件路径..."));
    layout->addWidget(mLineEdit, 1); // 占据大部分空间

    // 浏览按钮
    mBrowseBtn = new QPushButton(tr("浏览..."));
    mBrowseBtn->setFixedWidth(70);
    layout->addWidget(mBrowseBtn);

    // 连接信号槽
    connect(mBrowseBtn, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
    connect(mLineEdit, SIGNAL(textEdited(const QString &)),
            this, SLOT(onLineEditEdited(const QString &)));
}

// ============================================================
// 获取/设置路径
// ============================================================
QString FilePathSelector::filePath() const
{
    return mLineEdit->text();
}

void FilePathSelector::setFilePath(const QString &path)
{
    mLineEdit->setText(path);
}

// ============================================================
// 设置浏览按钮文字
// ============================================================
void FilePathSelector::setBrowseButtonText(const QString &text)
{
    mBrowseBtn->setText(text);
}

// ============================================================
// 设置占位文字
// ============================================================
void FilePathSelector::setPlaceholderText(const QString &text)
{
    mLineEdit->setPlaceholderText(text);
}

// ============================================================
// 设置文件过滤器
// ============================================================
void FilePathSelector::setFilter(const QString &filter)
{
    mFilter = filter;
}

// ============================================================
// 获取 QLineEdit 指针
// ============================================================
QLineEdit *FilePathSelector::lineEdit() const
{
    return mLineEdit;
}

// ============================================================
// 打开文件对话框
// ============================================================
void FilePathSelector::onBrowseClicked()
{
    emit browseClicked();

    QString path = QFileDialog::getOpenFileName(this, tr("选择文件"),
                                                 mLineEdit->text(), mFilter);
    if (!path.isEmpty()) {
        mLineEdit->setText(path);
        emit filePathChanged(path);
    }
}

// ============================================================
// 路径编辑完成
// ============================================================
void FilePathSelector::onLineEditEdited(const QString &text)
{
    emit filePathChanged(text);
}

// ============================================================
// SearchBox 构造函数
// ============================================================
SearchBox::SearchBox(QWidget *parent)
    : QWidget(parent)
    , mLineEdit(nullptr)
    , mClearBtn(nullptr)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // 搜索图标（使用 Unicode 字符模拟）
    // 创建一个只读的图标 label
    mLineEdit = new QLineEdit;
    mLineEdit->setPlaceholderText(tr("搜索..."));
    layout->addWidget(mLineEdit, 1);

    // 清除按钮（默认隐藏）
    mClearBtn = new QPushButton(tr("✕"));
    mClearBtn->setFixedSize(24, 24);
    mClearBtn->setToolTip(tr("清除"));
    mClearBtn->hide();
    layout->addWidget(mClearBtn);

    // 连接信号槽
    connect(mLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(onTextChanged(const QString &)));
    connect(mLineEdit, SIGNAL(returnPressed()),
            this, SLOT(onReturnPressed()));
    connect(mClearBtn, SIGNAL(clicked()), this, SLOT(onClearClicked()));
}

// ============================================================
// 获取/设置搜索文本
// ============================================================
QString SearchBox::text() const
{
    return mLineEdit->text();
}

void SearchBox::setText(const QString &text)
{
    mLineEdit->setText(text);
}

// ============================================================
// 设置占位文字
// ============================================================
void SearchBox::setPlaceholderText(const QString &text)
{
    mLineEdit->setPlaceholderText(text);
}

// ============================================================
// 文本变化 —— 控制清除按钮显示/隐藏
// ============================================================
void SearchBox::onTextChanged(const QString &text)
{
    mClearBtn->setVisible(!text.isEmpty());
    emit textChanged(text);
}

// ============================================================
// 清除按钮点击
// ============================================================
void SearchBox::onClearClicked()
{
    mLineEdit->clear();
    mLineEdit->setFocus();
    emit cleared();
}

// ============================================================
// 回车键触发搜索
// ============================================================
void SearchBox::onReturnPressed()
{
    emit searchRequested(mLineEdit->text());
}
