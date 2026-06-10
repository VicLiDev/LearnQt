//****************************************************************
//需要在.pro文件中添加语句
//QT += axcontainer
//****************************************************************
#pragma once
#include <QObject>
#include <QAxObject>
#include <QtCore>

enum lineFormat
{
	LEFT,
	RIGHT,
	MIDDLE,
};

enum tableFormat
{
	FIXATION = 0,
	BASED_ON_CONTENT = 1,
	BASED_ON_WINDOW = 2,
};

class WordEngine : public QObject
{
	Q_OBJECT
public:
	WordEngine();
	~WordEngine();

#pragma region 文件操作

	//****************************************************************
	//FunctionName : newFile 新建word文档
	//Param : bool visible 是否显示文档(true 显示word文档，false 不显示word文档)
	//****************************************************************
	bool newFile(bool visible = false);

	//****************************************************************
	//FunctionName : save 保存word文档
	//Param : QString savePath 文件保存目录
	//****************************************************************
	bool save(QString savePath);

	//****************************************************************
	//FunctionName : close 关闭word文档（关闭前一定要保存）
	//****************************************************************
	void close();

#pragma endregion 文件操作

#pragma region 格式控制

	//****************************************************************
	//FunctionName : setLineFormat 设置对齐方式
	//Param : lineFormat type 对齐方式（LEFT 左对齐，RIGHT 右对齐，MIDDLE 居中）
	//****************************************************************
	bool setLineFormat(lineFormat type);

	//****************************************************************
	//FunctionName : addEnter 换行
	//****************************************************************
	bool addEnter();

	//****************************************************************
	//FunctionName : moveDownSelection 使光标跳出表格（在为表格单元添加字符串后需要使用此方法使光标跳出表格）
	//****************************************************************
	bool moveDownSelection();

#pragma endregion 格式控制

#pragma region 文本和图片

	//****************************************************************
	//FunctionName : addText 添加文本
	//Param : QString text 文本
	//****************************************************************
	bool addText(QString text);

	//****************************************************************
	//FunctionName : setFontSize 设置字体大小（需要在添加文本前设置）
	//Param : int fontSize 字体大小
	//****************************************************************
	bool setFontSize(int fontSize);	

	//****************************************************************
	//FunctionName : setFontName 设置字体（需要在添加文本前设置）
	//Param : QString& fontName 字体名
	//****************************************************************
	bool setFontName(QString& fontName);

	//****************************************************************
	//FunctionName : setFontName 设置字体加粗（需要在添加文本前设置）
	//Param : bool flag 是否加粗（true 加粗，false 不加粗）
	//****************************************************************
	bool setFontBold(bool flag);

	//****************************************************************
	//FunctionName : addPicture 添加图片
	//Param : QString& filePath 图片目录
	//****************************************************************
	bool addPicture(QString& filePath);

#pragma endregion 文本和图片

#pragma region 表格
	
	//****************************************************************
	//FunctionName : addPicture 添加表格
	//Param : int row 行数
	//Param : int column 列数
	//Param : tableFormat type 表格生成方式（FIXATION 固定大小，BASED_ON_CONTENT 根据文字调整大小，BASED_ON_WINDOW 根据窗口调整大小）
	//****************************************************************
	QAxObject *addTable(int row, int column, tableFormat type);

	//****************************************************************
	//FunctionName : setTableCellString 为表格单元添加字符串
	//Param : QAxObject *table 表格对象指针
	//Param : int row 行号（从1开始）
	//Param : int column 列号（从1开始）
	//Param : QString text 字符串内容
	//****************************************************************
	bool setTableCellString(QAxObject *table, int row, int column, QString text);

	//****************************************************************
	//FunctionName : setTableCellFontBold 设置表格单元字体加粗
	//Param : QAxObject *table 表格对象指针
	//Param : int row 行号（从1开始）
	//Param : int column 列号（从1开始）
	//Param : bool isBold  是否加粗（true 加粗，false 不加粗）
	//****************************************************************
	bool setTableCellFontBold(QAxObject *table, int row, int column, bool isBold);

	//****************************************************************
	//FunctionName : setTableCellFontBold 设置表格单元字体大小
	//Param : QAxObject *table 表格对象指针
	//Param : int row 行号（从1开始）
	//Param : int column 列号（从1开始）
	//Param : int size  字体大小
	//****************************************************************
	bool setTableCellFontSize(QAxObject *table, int row, int column, int size);

	//****************************************************************
	//FunctionName : setTableCellFontName 设置表格单元字体
	//Param : QAxObject *table 表格对象指针
	//Param : int row 行号（从1开始）
	//Param : int column 列号（从1开始）
	//Param : QString& fontName 字体名
	//****************************************************************
	bool setTableCellFontName(QAxObject *table, int row, int column, QString& fontName);

	//****************************************************************
	//FunctionName : setTableCellFontName 设置表格列宽
	//Param : QAxObject *table 表格对象指针
	//Param : int column 列号（从1开始）
	//Param : int width 宽度
	//****************************************************************
	bool setTableColumnWidth(QAxObject *table, int column, int width);

	//****************************************************************
	//FunctionName : setTableCellFontName 设置表格列行高
	//Param : QAxObject *table 表格对象指针
	//Param : int row 行号（从1开始）
	//Param : int height 高度
	//****************************************************************
	bool setTableColumnHeight(QAxObject *table, int row, int height);

	//****************************************************************
	//FunctionName : setTableCellFontName 合并表格单元格
	//Param : QAxObject *table 表格对象指针
	//Param : int startRow 起始行号（从1开始）
	//Param : int startColumn 起始列号（从1开始）
	//Param : int endRow 终止行号（从1开始）
	//Param : int endColumn 终止列号（从1开始）
	//****************************************************************
	void mergeTableCells(QAxObject *table, int startRow, int startColumn, int endRow, int endColumn);

#pragma endregion 表格

private:
	QAxObject *m_pWord;
	QAxObject *m_pWorkDocument;
	bool m_bIsOpen;
	bool m_bNewFile;
};


