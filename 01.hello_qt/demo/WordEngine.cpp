#include "WordEngine.h"

#include "qt_windows.h"
WordEngine::WordEngine()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_pWord = NULL;
	m_pWorkDocument = NULL;

	m_bIsOpen = false;
	m_bNewFile = false;

	HRESULT result = OleInitialize(0);
	//OleInitialize是初始化Ole的运行环境，Ole是在Com的基础上作的扩展，是ActiveX运行的基础，OleInitialize肯定会调用CoInitialize

	if (result != S_OK && result != S_FALSE)
	{
		qDebug() << QString("Could not initialize OLE (error %x)").arg((unsigned int)result);
	}
}

WordEngine::~WordEngine()
{
	OleUninitialize();
}


bool WordEngine::setTableColumnWidth(QAxObject *table, int column, int width)
{
	if (!table)
	{
		return false;
	}
	table->querySubObject("Columns(int)", column)->setProperty("Width", width);
	return true;
}

bool WordEngine::setTableColumnHeight(QAxObject *table, int row, int height)
{
	if (!table)
	{
		return false;
	}
	table->querySubObject("Rows(int)", row)->setProperty("Height", height);
	return true;
}



bool WordEngine::newFile(bool visible)
{
	m_pWord = new QAxObject();
	bool bFlag = m_pWord->setControl("word.Application");
	if (!bFlag)
	{
		return false;
	}
	m_pWord->setProperty("Visible", visible);

	QAxObject *document = m_pWord->querySubObject("Documents");
	if (!document)
	{
		return false;
	}

	document->dynamicCall("Add(void)");

	m_pWorkDocument = m_pWord->querySubObject("ActiveDocument");
	if (m_pWorkDocument)
		m_bIsOpen = true;
	else
		m_bIsOpen = false;

	return m_bIsOpen;
}


bool WordEngine::addText(QString text)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	selection->dynamicCall("TypeText(const QString&)", text);

	return true;
}

bool WordEngine::setLineFormat(lineFormat type)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	switch (type)
	{
	case LEFT:
		selection->querySubObject("ParagraphFormat")->setProperty("Alignment", "wdAlignParagraphJustify");
		return true;
		break;
	case RIGHT:
		selection->querySubObject("ParagraphFormat")->setProperty("Alignment", "wdAlignParagraphRight");
		return true;
		break;
	case MIDDLE:
		selection->querySubObject("ParagraphFormat")->setProperty("Alignment", "wdAlignParagraphCenter");
		return true;
		break;
	default:
		break;
	}
	return false;
}

bool WordEngine::addEnter()
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	selection->dynamicCall("TypeParagraph(void)");

	return true;
}

QAxObject * WordEngine::addTable(int row, int column, tableFormat type)
{
	QAxObject *selection = m_pWord->querySubObject("Selection");
	selection->querySubObject("ParagraphFormat")->dynamicCall("Alignment", "wdAlignParagraphCenter");
	QAxObject *range = selection->querySubObject("Range");
	QAxObject *tables = m_pWorkDocument->querySubObject("Tables");
	QAxObject *table = tables->querySubObject("Add(QVariant,int,int)", range->asVariant(), row, column);
	table->dynamicCall("AutoFitBehavior(WdAutoFitBehavior)", type);
	for (int i = 1; i <= 6; i++)
	{
		QString str = QString("Borders(-%1)").arg(i);
		QAxObject *borders = table->querySubObject(str.toLatin1().constData());
		borders->dynamicCall("SetLineStyle(int)", 1);
	}
	return table;
	
}

bool WordEngine::setFontSize(int fontSize)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	selection->querySubObject("Font")->setProperty("Size", fontSize);
	return true;
}

bool WordEngine::setFontName(QString& fontName)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	selection->querySubObject("Font")->setProperty("Name", fontName);
	return true;
}

bool WordEngine::setFontBold(bool flag)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	selection->querySubObject("Font")->setProperty("Bold", flag);
	return true;
}

bool WordEngine::setTableCellString(QAxObject *table, int row, int column, QString text)
{
	if (!table)
		return false;
	QAxObject *cell = table->querySubObject("Cell(int,int)", row, column);
	if (!cell)
		return false;
	cell->dynamicCall("Select(void)");
	cell->querySubObject("Range")->setProperty("Text", text);
	return true;

}

bool WordEngine::moveDownSelection()
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}
	QVariantList params;
	params.append(6);
	params.append(0);
	selection->dynamicCall("EndOf(QVariant&,QVariant&)", params).toInt();
}

bool WordEngine::addPicture(QString& filePath)
{
	if (!m_pWord)
	{
		return false;
	}
	QAxObject* selection = m_pWord->querySubObject("Selection");
	if (!selection)
	{
		return false;
	}

	QAxObject *Inlineshapes = m_pWorkDocument->querySubObject("InlineShapes");
	Inlineshapes->dynamicCall("AddPicture(const QString&)", filePath);
	delete Inlineshapes;
	return true;
}

bool WordEngine::save(QString savePath)
{
	if (m_bIsOpen && m_pWorkDocument)
	{
		if (m_bNewFile)
		{
			m_pWorkDocument->dynamicCall("Save()");
			return true;
		}
		else
		{
			m_pWorkDocument->dynamicCall("SaveAs (const QString&)", savePath);
			return true;
		}
	}
	return false;
}

void WordEngine::close()
{
	if (m_pWord){
		m_pWord->setProperty("DisplayAlerts", true);
	}
	if (m_pWorkDocument){
		m_pWorkDocument->dynamicCall("Close(bool)", true);
	}
	if (m_pWord){
		m_pWord->dynamicCall("Quit()");
	}
	if (m_pWord)
	{
		delete m_pWord;
	}
	m_pWorkDocument = NULL;
	m_pWord = NULL;

	m_bIsOpen = false;
	m_bNewFile = false;

}

void WordEngine::mergeTableCells(QAxObject *table, int startRow, int startColumn, int endRow, int endColumn)
{
	if (!table)
	{
		return;
	}
	QAxObject* StartCell = table->querySubObject("Cell(int, int)", startRow, startColumn);
	QAxObject* EndCell = table->querySubObject("Cell(int, int)", endRow, endColumn);
	if (!StartCell)
	{
		return;
	}
	if (!EndCell)
	{
		return;
	}
	StartCell->querySubObject("Merge(QAxObject *)", EndCell->asVariant());
}

bool WordEngine::setTableCellFontBold(QAxObject *table, int row, int column, bool isBold)
{
	if (!table)
		return false;
	table->querySubObject("Cell(int,int )", row, column)->querySubObject("Range")->dynamicCall("SetBold(int)", isBold);
	return true;
}
bool WordEngine::setTableCellFontSize(QAxObject *table, int row, int column, int size)
{
	if (!table)
		return false;
	table->querySubObject("Cell(int,int)", row, column)->querySubObject("Range")->querySubObject("Font")->setProperty("Size", size);
	return true;
}
bool WordEngine::setTableCellFontName(QAxObject *table, int row, int column, QString& fontName)
{
	if (!table)
		return false;
	table->querySubObject("Cell(int,int )", row, column)->querySubObject("Range")->querySubObject("Font")->setProperty("Name", fontName);
	return true;
}
