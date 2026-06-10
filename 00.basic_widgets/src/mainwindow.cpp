#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAxWidget>
#include <QAxObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //***********************************************
    //Qt生成word有两种方法，一种是使用模板，一种是直接创建，使用模板需要预先添加好标签
    //两个不错的博客：
    //        使用模板：https://blog.csdn.net/vinson0526/article/details/11094877
    //        直接创建：https://blog.csdn.net/nameofhsw/article/details/17955373
    //        一个不错的demo：https://blog.csdn.net/zhangfuliang123/article/details/53413477
    //直接创建这个博客里只介绍创建保存的方法，关于如何添加内容，参见使用模板的博客
    //使用模板的博客需要使用 QAxObject *bookmark_text=document->querySubObject("Bookmarks(label1)"); 来获取标签

    //第一步：
    //隐式的打开一个word应用程序
    QAxWidget word("Word.Application");
    word.setProperty("Visible", false);

    //第二步：根据模板创建word，或者直接通过指令创建word，两种方法只有一条指令的区别，下边已经标注
    //获取所有工作文档
    QAxObject *documents = word.querySubObject("Documents");
    //创建一个word文档
    documents->dynamicCall("Add (void)");   //直接创建word的方法
    //documents->dynamicCall("Add(QString)",QString("D:/template1.dot")); //使用模板创建的方法，路径为模板路径
    //获取当前激活的文档
    QAxObject *document = word.querySubObject("ActiveDocument");

    //第三步：写入内容 两种方法此步不同，可以参考上边的链接，这里写的是不用模板的方法
    //=======写入文本内容
    //+++第一行，插入文本
    QAxObject *selection = word.querySubObject("Selection");
    selection->dynamicCall("TypeText(const QString&)","hello world");
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    //selection->dynamicCall("InsertAfter(QString&)", "\r\n");  //换行 网上的方法 不知道\r有啥用 也不知道跟上边那句有啥区别
    //+++第二行，设置段落对齐
    selection->dynamicCall("TypeText(const QString&)","第二行");
    selection->querySubObject("ParagraphFormat")->dynamicCall("Alignment", "wdAlignParagraphCenter"); //左对齐：wdAlignParagraphLeft，右对齐：wdAlignParagraphCenter
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    //+++第三行，设置字体
    selection->querySubObject("Font")->dynamicCall("Bold","wdToggle"); //加粗
    selection->querySubObject("Font")->dynamicCall("Italic","wdToggle"); //倾斜
    selection->querySubObject("Font")->dynamicCall("Name","Adobe 楷体 Std R"); //楷体，字体可以在word中录制宏之后查看
//    selection->querySubObject("Font")->dynamicCall("Size",22); //设置字号
//    selection->querySubObject("Font")->dynamicCall("Color",-671023105); //设置字体颜色
//    selection->querySubObject("Range")->dynamicCall("HighlightColorIndex","wdRed"); //设置字体背景颜色
    selection->dynamicCall("TypeText(const QString&)","第三行");
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    //======插入表格
    //插入表格
    QAxObject *range = selection->querySubObject("Range");
    QAxObject *tables = document->querySubObject("Tables");
    QAxObject *table = tables->querySubObject("Add(QVariant,int,int)",range->asVariant(),4,5);  //两行三列
    //表格自动拉伸列 0固定  1根据内容调整  2 根据窗口调整
    table->dynamicCall("AutoFitBehavior(WdAutoFitBehavior)", 0);
    //设置表格类型
    selection->querySubObject("Tables(1)")->dynamicCall("Style","网格型");
    //合并单元格
//    QAxObject* StartCell = table->querySubObject("Cell(int, int)", startRow, startColumn); //startRow:开始行 startColumn:开始列
//	QAxObject* EndCell = table->querySubObject("Cell(int, int)", endRow, endColumn); //endRow:结束行 endColumn:结束列
//    StartCell->querySubObject("Merge(QAxObject *)", EndCell->asVariant());
    //插入文本
    table->querySubObject("Cell(int,int)",1,1)->querySubObject("Range")->dynamicCall("SetText(QString)",QString("插入内容"));
    table->querySubObject("Cell(int,int)",1,1)->querySubObject("Range")->dynamicCall("Bold","wdToggle");  //加粗
    table->querySubObject("Cell(int,int)",1,1)->querySubObject("Range")->querySubObject("Font")->dynamicCall("Italic","wdToggle"); //倾斜
    table->querySubObject("Cell(int,int)",1,1)->querySubObject("Range")->querySubObject("Font")->dynamicCall("Size",22);  //字号
    table->querySubObject("Cell(int,int)",1,1)->querySubObject("Range")->querySubObject("Font")->dynamicCall("Color",-671023105); //字体颜色
    //跳出表格
    selection->dynamicCall("MoveDown(QVariant&)",11);  //调到文本最后，不知道为啥，只有设置7和11可以  4 7 11
    //selection->dynamicCall("EndKey(QString&)","wdStory");
//    QVariantList params;
//    params.append(6);
//    params.append(0);
//    selection->dynamicCall("EndOf(QVariant&,QVariant&)", params).toInt();
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    selection->dynamicCall("TypeParagraph(void)");  //正经换行
    //======插入图片
    selection->querySubObject("InlineShapes")->dynamicCall("AddPicture(const QString&)",QString("D:\\123.jpg"));

    //第四步:保存文档 两种方法此步相同
    //设置保存
    QVariant newFileName("D:/test.doc"); //存放位置和名称
    QVariant fileFormat(1); //文件格式
    QVariant LockComments(false);
    QVariant Password("");     //设置打开密码
    QVariant recent(true);
    QVariant writePassword("");
    QVariant ReadOnlyRecommended(false);
    document->querySubObject("SaveAs(const QVariant&, const QVariant&,const QVariant&, const QVariant&, const QVariant&, const QVariant&,const QVariant&)",    newFileName, fileFormat, LockComments, Password, recent, writePassword, ReadOnlyRecommended);
    //关闭文档
    document->dynamicCall("Close (boolean)", true);
    //退出
    word.dynamicCall("Quit (void)");
}

MainWindow::~MainWindow()
{
    delete ui;
}
