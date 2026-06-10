#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;
class CircleButton;
class GaugeWidget;
class QSlider;
class QLabel;
class QTableView;
class QStandardItemModel;
class StarDelegate;
class FilePathSelector;
class SearchBox;
class QLabel;

// ============================================================
// 主窗口 —— 包含四个 Tab 页的自定义控件演示
// ============================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Tab 控件
    QTabWidget *mTabWidget;

    // ---- Tab 1: 自定义绘制控件 ----
    CircleButton *mCircleBtn1;      // 普通圆形按钮
    CircleButton *mCircleBtn2;      // 可选中圆形按钮
    CircleButton *mCircleBtn3;      // 可选中圆形按钮
    QLabel *mCircleStatusLabel;      // 状态标签

    // ---- Tab 2: 进度仪表盘 ----
    GaugeWidget *mGaugeWidget;       // 仪表盘控件
    QSlider *mGaugeSlider;          // 控制仪表盘值的滑块
    QLabel *mGaugeValueLabel;       // 仪表盘数值标签

    // ---- Tab 3: 自定义 Delegate ----
    QTableView *mTableView;         // 表格视图
    QStandardItemModel *mTableModel; // 表格数据模型
    StarDelegate *mStarDelegate;     // 星级委托

    // ---- Tab 4: 组合控件 ----
    FilePathSelector *mPathSelector; // 文件路径选择器
    QLabel *mPathDisplayLabel;       // 路径显示标签
    SearchBox *mSearchBox;           // 搜索框
    QLabel *mSearchResultLabel;     // 搜索结果标签

    // 初始化各 Tab 页
    void setupTab1CircleButton(QWidget *parent);
    void setupTab2GaugeWidget(QWidget *parent);
    void setupTab3Delegate(QWidget *parent);
    void setupTab4CompositeWidget(QWidget *parent);
};

#endif // MAINWINDOW_H
