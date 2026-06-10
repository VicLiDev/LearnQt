#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class QTextEdit;
class QPushButton;
class QLineEdit;
class QSpinBox;

/* ========== Tab1: JSON 读写 ========== */
class JsonReadWriteTab : public QWidget
{
    Q_OBJECT
public:
    explicit JsonReadWriteTab(QWidget *parent = nullptr);

private slots:
    void onCreateJson();
    void onSaveJson();
    void onLoadJson();
    void onParseJson();

private:
    QTextEdit *m_display;
};

/* ========== Tab2: JSON 高级 ========== */
class JsonAdvancedTab : public QWidget
{
    Q_OBJECT
public:
    explicit JsonAdvancedTab(QWidget *parent = nullptr);

private slots:
    void onParseNested();
    void onModifyJson();
    void onDeleteKey();
    void onValidateJson();
    void onVariantConvert();

private:
    QLineEdit *m_pathInput;
    QLineEdit *m_keyInput;
    QLineEdit *m_valueInput;
    QTextEdit *m_display;
};

/* ========== Tab3: XML 读写 ========== */
class XmlReadWriteTab : public QWidget
{
    Q_OBJECT
public:
    explicit XmlReadWriteTab(QWidget *parent = nullptr);

private slots:
    void onCreateXml();
    void onSaveXml();
    void onLoadXml();
    void onParseXml();

private:
    QTextEdit *m_display;
};

/* ========== Tab4: XML 搜索 ========== */
class XmlSearchTab : public QWidget
{
    Q_OBJECT
public:
    explicit XmlSearchTab(QWidget *parent = nullptr);

private slots:
    void onSearchByTag();
    void onTraverseElements();
    void onReadAttributes();
    void onModifyXml();

private:
    QLineEdit *m_tagInput;
    QTextEdit *m_display;
};

/* ========== Tab5: JSON vs XML 对比 ========== */
class JsonXmlCompareTab : public QWidget
{
    Q_OBJECT
public:
    explicit JsonXmlCompareTab(QWidget *parent = nullptr);

private slots:
    void onGenerateBoth();
    void onBenchmark();
    void onShowSummary();

private:
    QSpinBox *m_dataCountSpin;
    QTextEdit *m_jsonDisplay;
    QTextEdit *m_xmlDisplay;
    QTextEdit *m_resultDisplay;
};

/* ========== 主窗口 ========== */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QTabWidget *m_tabWidget;
};

#endif // MAINWINDOW_H
