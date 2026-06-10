#ifndef FILE_PATH_SELECTOR_H
#define FILE_PATH_SELECTOR_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QHBoxLayout;

// ============================================================
// 组合控件 —— 带浏览按钮的文件路径选择器
// 封装 QLineEdit + QPushButton，提供路径选择功能
// 通过信号槽接口与外部交互
// ============================================================
class FilePathSelector : public QWidget
{
    Q_OBJECT

public:
    explicit FilePathSelector(QWidget *parent = nullptr);

    // 获取/设置当前路径
    QString filePath() const;
    void setFilePath(const QString &path);

    // 设置浏览按钮文字
    void setBrowseButtonText(const QString &text);

    // 设置占位文字提示
    void setPlaceholderText(const QString &text);

    // 设置文件对话框过滤器
    void setFilter(const QString &filter);

    // 获取 QLineEdit 指针（可进一步自定义）
    QLineEdit *lineEdit() const;

signals:
    // 路径改变信号
    void filePathChanged(const QString &path);

    // 浏览按钮点击信号
    void browseClicked();

private slots:
    // 打开文件对话框
    void onBrowseClicked();

    // 路径文本编辑完成
    void onLineEditEdited(const QString &text);

private:
    QLineEdit *mLineEdit;     // 路径输入框
    QPushButton *mBrowseBtn;   // 浏览按钮
    QString mFilter;            // 文件对话框过滤器
};

// ============================================================
// 组合控件 —— 搜索框 + 清除按钮
// 封装 QLineEdit + 清除按钮，提供搜索输入功能
// ============================================================
class SearchBox : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBox(QWidget *parent = nullptr);

    // 获取/设置搜索文本
    QString text() const;
    void setText(const QString &text);

    // 设置占位文字
    void setPlaceholderText(const QString &text);

signals:
    // 搜索文本变化信号
    void textChanged(const QString &text);

    // 搜索触发信号（按下回车）
    void searchRequested(const QString &text);

    // 清除按钮点击信号
    void cleared();

private slots:
    void onTextChanged(const QString &text);
    void onClearClicked();
    void onReturnPressed();

private:
    QLineEdit *mLineEdit;     // 搜索输入框
    QPushButton *mClearBtn;   // 清除按钮
};

#endif // FILE_PATH_SELECTOR_H
