#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QVariant>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>

#include <QFile>
#include <QTextStream>
#include <QDateTime>

// 前向声明静态辅助函数（在 XmlSearchTab 中使用）
static QString traverseNode(QDomNode node, int depth);

// ============================================================================
// Tab1: JSON 读写
// ============================================================================

JsonReadWriteTab::JsonReadWriteTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 按钮区域
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCreate = new QPushButton("创建 JSON");
    QPushButton *btnSave = new QPushButton("保存到文件");
    QPushButton *btnLoad = new QPushButton("从文件加载");
    QPushButton *btnParse = new QPushButton("解析 JSON");
    btnLayout->addWidget(btnCreate);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnLoad);
    btnLayout->addWidget(btnParse);
    layout->addLayout(btnLayout);

    // 显示区域
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(new QLabel("JSON 输出："));
    layout->addWidget(m_display);

    // 信号连接
    connect(btnCreate, &QPushButton::clicked, this, &JsonReadWriteTab::onCreateJson);
    connect(btnSave, &QPushButton::clicked, this, &JsonReadWriteTab::onSaveJson);
    connect(btnLoad, &QPushButton::clicked, this, &JsonReadWriteTab::onLoadJson);
    connect(btnParse, &QPushButton::clicked, this, &JsonReadWriteTab::onParseJson);
}

void JsonReadWriteTab::onCreateJson()
{
    // 创建 JSON 对象
    QJsonObject rootObj;
    rootObj["名称"] = "Qt5 JSON 演示";
    rootObj["版本"] = "1.0";
    rootObj["日期"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    // 创建 JSON 数组
    QJsonArray features;
    features.append("QJsonObject 操作");
    features.append("QJsonArray 遍历");
    features.append("QJsonDocument 序列化");
    features.append("文件读写");
    rootObj["功能列表"] = features;

    // 嵌套 JSON 对象
    QJsonObject authorObj;
    authorObj["姓名"] = "张三";
    authorObj["年龄"] = 30;
    authorObj["语言"] = QString::fromUtf8("C++");
    rootObj["作者"] = authorObj;

    // 序列化为 JSON 文档
    QJsonDocument doc(rootObj);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Indented);
    m_display->setPlainText(QString::fromUtf8(jsonBytes));
}

void JsonReadWriteTab::onSaveJson()
{
    if (m_display->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "提示", "请先创建 JSON 数据！");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, "保存 JSON 文件", "data.json", "JSON Files (*.json);;All Files (*)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    file.write(m_display->toPlainText().toUtf8());
    file.close();
    QMessageBox::information(this, "成功", "JSON 文件已保存到：" + filePath);
}

void JsonReadWriteTab::onLoadJson()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "打开 JSON 文件", QString(), "JSON Files (*.json);;All Files (*)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    m_display->setPlainText(QString::fromUtf8(data));
}

void JsonReadWriteTab::onParseJson()
{
    QString text = m_display->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先加载 JSON 数据！");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "解析错误",
            QString("JSON 解析失败：\n错误位置：%1\n错误信息：%2")
                .arg(parseError.offset).arg(parseError.errorString()));
        return;
    }

    // 解析并显示结果
    QString result;
    QJsonObject rootObj = doc.object();

    result += "===== JSON 解析结果 =====\n\n";

    // 遍历顶层键值对
    result += "--- 顶层键值对 ---\n";
    for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
        QString key = it.key();
        QJsonValue val = it.value();

        if (val.isString()) {
            result += QString("  %1 : \"%2\"\n").arg(key, val.toString());
        } else if (val.isDouble()) {
            result += QString("  %1 : %2\n").arg(key).arg(val.toDouble());
        } else if (val.isArray()) {
            result += QString("  %1 : [数组, %2 个元素]\n").arg(key).arg(val.toArray().size());
            QJsonArray arr = val.toArray();
            for (int i = 0; i < arr.size(); ++i) {
                result += QString("    [%1] : %2\n").arg(i).arg(arr[i].toString());
            }
        } else if (val.isObject()) {
            result += QString("  %1 : [对象]\n").arg(key);
            QJsonObject subObj = val.toObject();
            for (auto subIt = subObj.begin(); subIt != subObj.end(); ++subIt) {
                result += QString("    %1 : %2\n").arg(subIt.key(), subIt.value().toString());
            }
        }
    }

    m_display->setPlainText(result);
}

// ============================================================================
// Tab2: JSON 高级
// ============================================================================

JsonAdvancedTab::JsonAdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 操作区域
    QGridLayout *grid = new QGridLayout();

    QPushButton *btnParseNested = new QPushButton("解析嵌套 JSON");
    grid->addWidget(btnParseNested, 0, 0);

    // JSON Path 查询
    grid->addWidget(new QLabel("JSON Path（如：作者.姓名）："), 1, 0);
    m_pathInput = new QLineEdit();
    m_pathInput->setPlaceholderText("输入路径，用 . 分隔");
    grid->addWidget(m_pathInput, 1, 1);
    QPushButton *btnQuery = new QPushButton("查询");
    grid->addWidget(btnQuery, 1, 2);

    // 修改 JSON
    grid->addWidget(new QLabel("键名："), 2, 0);
    m_keyInput = new QLineEdit();
    grid->addWidget(m_keyInput, 2, 1);
    grid->addWidget(new QLabel("值："), 3, 0);
    m_valueInput = new QLineEdit();
    grid->addWidget(m_valueInput, 3, 1);
    QPushButton *btnModify = new QPushButton("修改");
    grid->addWidget(btnModify, 3, 2);
    QPushButton *btnDelete = new QPushButton("删除键");
    grid->addWidget(btnDelete, 3, 3);

    // 校验和转换
    QPushButton *btnValidate = new QPushButton("校验 JSON");
    QPushButton *btnVariant = new QPushButton("QVariant 转换");
    grid->addWidget(btnValidate, 4, 0);
    grid->addWidget(btnVariant, 4, 1);

    layout->addLayout(grid);

    // 显示区域
    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(m_display);

    // 信号连接
    connect(btnParseNested, &QPushButton::clicked, this, &JsonAdvancedTab::onParseNested);
    connect(btnQuery, &QPushButton::clicked, this, &JsonAdvancedTab::onDeleteKey);
    connect(btnModify, &QPushButton::clicked, this, &JsonAdvancedTab::onModifyJson);
    connect(btnDelete, &QPushButton::clicked, this, &JsonAdvancedTab::onDeleteKey);
    connect(btnValidate, &QPushButton::clicked, this, &JsonAdvancedTab::onValidateJson);
    connect(btnVariant, &QPushButton::clicked, this, &JsonAdvancedTab::onVariantConvert);
}

void JsonAdvancedTab::onParseNested()
{
    // 构建嵌套 JSON
    QJsonObject company;
    company["名称"] = "科技有限公司";
    company["成立年份"] = 2020;

    QJsonObject address;
    address["城市"] = "北京";
    address["区"] = "海淀区";
    address["街道"] = "中关村大街1号";
    company["地址"] = address;

    QJsonArray departments;
    QJsonObject dept1;
    dept1["名称"] = "研发部";
    dept1["人数"] = 50;
    QJsonArray skills1;
    skills1.append("C++");
    skills1.append("Qt");
    skills1.append("Python");
    dept1["技术栈"] = skills1;
    departments.append(dept1);

    QJsonObject dept2;
    dept2["名称"] = "测试部";
    dept2["人数"] = 20;
    departments.append(dept2);

    company["部门"] = departments;

    QJsonDocument doc(company);
    QByteArray json = doc.toJson(QJsonDocument::Indented);

    QString result = QString::fromUtf8(json);
    result += "\n\n===== 嵌套解析 =====\n";
    result += QString("公司名称：%1\n").arg(company["名称"].toString());
    result += QString("成立年份：%1\n").arg(company["成立年份"].toInt());

    // 解析嵌套地址
    QJsonObject addr = company["地址"].toObject();
    result += QString("城市：%1, 区：%2, 街道：%3\n")
        .arg(addr["城市"].toString(), addr["区"].toString(), addr["街道"].toString());

    // 解析嵌套部门数组
    QJsonArray depts = company["部门"].toArray();
    for (int i = 0; i < depts.size(); ++i) {
        QJsonObject d = depts[i].toObject();
        result += QString("\n部门[%1]：%2, 人数：%3\n")
            .arg(i).arg(d["名称"].toString()).arg(d["人数"].toInt());
        if (d.contains("技术栈")) {
            QJsonArray skills = d["技术栈"].toArray();
            for (int j = 0; j < skills.size(); ++j) {
                result += QString("  技术栈[%1]：%2\n").arg(j).arg(skills[j].toString());
            }
        }
    }

    m_display->setPlainText(result);
}

void JsonAdvancedTab::onModifyJson()
{
    // 创建示例 JSON 并修改
    QJsonObject obj;
    obj["名称"] = "原始值";
    obj["数量"] = 10;

    m_display->append("修改前：" + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));

    // 修改值
    QString key = m_keyInput->text().trimmed();
    QString val = m_valueInput->text().trimmed();
    if (!key.isEmpty()) {
        obj[key] = val;
    }

    m_display->append("修改后：" + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void JsonAdvancedTab::onDeleteKey()
{
    QString path = m_pathInput->text().trimmed();
    if (path.isEmpty()) {
        m_display->append("[查询] 请输入 JSON Path");
        return;
    }

    // 简化版 JSON Path 查询演示
    m_display->append(QString("[JSON Path] 查询路径: %1").arg(path));
    m_display->append("提示：Qt5 没有内置 JSON Path 支持，需手动解析路径。\n"
                      "实现思路：按 '.' 分割路径，逐层用 toObject()/toArray() 访问。\n"
                      "可使用第三方库如 jsoncons 或自己实现递归查找。");
}

void JsonAdvancedTab::onValidateJson()
{
    QString text = m_display->toPlainText().split("\n").first();
    QJsonParseError err;
    QJsonDocument::fromJson(text.toUtf8(), &err);

    if (err.error == QJsonParseError::NoError) {
        m_display->append("\n[校验结果] JSON 格式有效");
    } else {
        m_display->append(QString("\n[校验结果] JSON 格式无效：位置 %1，%2")
            .arg(err.offset).arg(err.errorString()));
    }
}

void JsonAdvancedTab::onVariantConvert()
{
    // JSON 与 QVariant 转换
    QVariantMap vmap;
    vmap["字符串"] = "你好";
    vmap["整数"] = 42;
    vmap["浮点数"] = 3.14;
    vmap["布尔"] = true;

    // QVariantMap -> QJsonObject
    QJsonObject obj = QJsonObject::fromVariantMap(vmap);
    QString json = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));

    // QJsonObject -> QVariantMap
    QVariantMap back = obj.toVariantMap();

    QString result;
    result += "===== QVariant 转换演示 =====\n\n";
    result += "原始 QVariantMap:\n";
    for (auto it = back.begin(); it != back.end(); ++it) {
        result += QString("  %1 (%2) : %3\n")
            .arg(it.key(), it.value().typeName()).arg(it.value().toString());
    }
    result += "\n转换为 JSON:\n" + json;

    m_display->setPlainText(result);
}

// ============================================================================
// Tab3: XML 读写
// ============================================================================

XmlReadWriteTab::XmlReadWriteTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCreate = new QPushButton("创建 XML");
    QPushButton *btnSave = new QPushButton("保存到文件");
    QPushButton *btnLoad = new QPushButton("从文件加载");
    QPushButton *btnParse = new QPushButton("解析 XML");
    btnLayout->addWidget(btnCreate);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnLoad);
    btnLayout->addWidget(btnParse);
    layout->addLayout(btnLayout);

    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(new QLabel("XML 输出："));
    layout->addWidget(m_display);

    connect(btnCreate, &QPushButton::clicked, this, &XmlReadWriteTab::onCreateXml);
    connect(btnSave, &QPushButton::clicked, this, &XmlReadWriteTab::onSaveXml);
    connect(btnLoad, &QPushButton::clicked, this, &XmlReadWriteTab::onLoadXml);
    connect(btnParse, &QPushButton::clicked, this, &XmlReadWriteTab::onParseXml);
}

void XmlReadWriteTab::onCreateXml()
{
    QDomDocument doc;
    // 添加 XML 处理指令
    QDomProcessingInstruction pi = doc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(pi);

    // 创建根元素
    QDomElement root = doc.createElement("图书库");
    root.setAttribute("名称", "Qt 学习图书");
    doc.appendChild(root);

    // 添加图书条目
    for (int i = 1; i <= 3; ++i) {
        QDomElement book = doc.createElement("图书");
        book.setAttribute("编号", QString::number(i));

        QDomElement title = doc.createElement("书名");
        QDomText titleText;
        if (i == 1) titleText = doc.createTextNode("C++ Primer");
        else if (i == 2) titleText = doc.createTextNode("Qt5 编程入门");
        else titleText = doc.createTextNode("设计模式");
        title.appendChild(titleText);
        book.appendChild(title);

        QDomElement author = doc.createElement("作者");
        if (i == 1) author.appendChild(doc.createTextNode("Stanley Lippman"));
        else if (i == 2) author.appendChild(doc.createTextNode("张三"));
        else author.appendChild(doc.createTextNode("Erich Gamma"));
        book.appendChild(author);

        QDomElement price = doc.createElement("价格");
        QDomElement year = doc.createElement("出版年份");
        if (i == 1) { price.appendChild(doc.createTextNode("128")); year.appendChild(doc.createTextNode("2012")); }
        else if (i == 2) { price.appendChild(doc.createTextNode("89")); year.appendChild(doc.createTextNode("2023")); }
        else { price.appendChild(doc.createTextNode("76")); year.appendChild(doc.createTextNode("1994")); }
        book.appendChild(price);
        book.appendChild(year);

        root.appendChild(book);
    }

    m_display->setPlainText(doc.toString(2));
}

void XmlReadWriteTab::onSaveXml()
{
    if (m_display->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "提示", "请先创建 XML 数据！");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, "保存 XML 文件", "data.xml", "XML Files (*.xml);;All Files (*)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    file.write(m_display->toPlainText().toUtf8());
    file.close();
    QMessageBox::information(this, "成功", "XML 文件已保存到：" + filePath);
}

void XmlReadWriteTab::onLoadXml()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "打开 XML 文件", QString(), "XML Files (*.xml);;All Files (*)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    m_display->setPlainText(QString::fromUtf8(data));
}

void XmlReadWriteTab::onParseXml()
{
    QString text = m_display->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先加载 XML 数据！");
        return;
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorCol;
    if (!doc.setContent(text, &errorMsg, &errorLine, &errorCol)) {
        QMessageBox::critical(this, "XML 解析错误",
            QString("错误位置：第%1行 第%2列\n错误信息：%3")
                .arg(errorLine).arg(errorCol).arg(errorMsg));
        return;
    }

    QString result;
    result += "===== XML 解析结果 =====\n\n";

    QDomElement root = doc.documentElement();
    result += QString("根元素：<%1>，属性 名称=\"%2\"\n\n").arg(root.tagName(), root.attribute("名称"));

    QDomNodeList books = root.elementsByTagName("图书");
    result += QString("共 %1 本图书：\n").arg(books.count());

    for (int i = 0; i < books.count(); ++i) {
        QDomElement book = books.at(i).toElement();
        result += QString("\n  [%1] 编号：%2\n")
            .arg(i + 1).arg(book.attribute("编号"));

        QDomElement title = book.firstChildElement("书名");
        QDomElement author = book.firstChildElement("作者");
        QDomElement price = book.firstChildElement("价格");
        QDomElement year = book.firstChildElement("出版年份");

        result += QString("    书名：%1\n").arg(title.text());
        result += QString("    作者：%1\n").arg(author.text());
        result += QString("    价格：%1 元\n").arg(price.text());
        result += QString("    出版年份：%1\n").arg(year.text());
    }

    m_display->setPlainText(result);
}

// ============================================================================
// Tab4: XML 搜索
// ============================================================================

XmlSearchTab::XmlSearchTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("标签名："));
    m_tagInput = new QLineEdit();
    m_tagInput->setPlaceholderText("如：书名、作者、价格");
    m_tagInput->setText("书名");
    searchLayout->addWidget(m_tagInput);
    QPushButton *btnSearch = new QPushButton("按标签搜索");
    searchLayout->addWidget(btnSearch);
    layout->addLayout(searchLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnTraverse = new QPushButton("遍历所有元素");
    QPushButton *btnAttrs = new QPushButton("读取属性");
    QPushButton *btnModify = new QPushButton("修改 XML");
    btnLayout->addWidget(btnTraverse);
    btnLayout->addWidget(btnAttrs);
    btnLayout->addWidget(btnModify);
    layout->addLayout(btnLayout);

    m_display = new QTextEdit();
    m_display->setReadOnly(true);
    m_display->setFont(QFont("Courier", 10));
    layout->addWidget(m_display);

    connect(btnSearch, &QPushButton::clicked, this, &XmlSearchTab::onSearchByTag);
    connect(btnTraverse, &QPushButton::clicked, this, &XmlSearchTab::onTraverseElements);
    connect(btnAttrs, &QPushButton::clicked, this, &XmlSearchTab::onReadAttributes);
    connect(btnModify, &QPushButton::clicked, this, &XmlSearchTab::onModifyXml);
}

void XmlSearchTab::onSearchByTag()
{
    // 构建示例 XML
    QDomDocument doc;
    QDomProcessingInstruction pi = doc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(pi);

    QDomElement root = doc.createElement("图书库");
    doc.appendChild(root);

    QStringList titles = QStringList() << "C++ Primer" << "Qt5 编程" << "设计模式";
    QStringList authors = QStringList() << "Lippman" << "张三" << "Gamma";

    for (int i = 0; i < titles.size(); ++i) {
        QDomElement book = doc.createElement("图书");
        book.setAttribute("编号", QString::number(i + 1));

        QDomElement t = doc.createElement("书名");
        t.appendChild(doc.createTextNode(titles[i]));
        book.appendChild(t);

        QDomElement a = doc.createElement("作者");
        a.appendChild(doc.createTextNode(authors[i]));
        book.appendChild(a);

        root.appendChild(book);
    }

    // 按标签名搜索（类似 XPath）
    QString tagName = m_tagInput->text().trimmed();
    QDomNodeList nodes = doc.elementsByTagName(tagName);

    QString result;
    result += QString("===== 按标签搜索 \"%1\" =====\n\n").arg(tagName);
    result += QString("找到 %1 个匹配节点：\n").arg(nodes.count());

    for (int i = 0; i < nodes.count(); ++i) {
        QDomElement elem = nodes.at(i).toElement();
        QString parentTag = elem.parentNode().toElement().tagName();
        result += QString("  [%1] <%2>（父元素：<%3>）= \"%4\"\n")
            .arg(i + 1).arg(elem.tagName(), parentTag, elem.text());
    }

    m_display->setPlainText(result);
}

void XmlSearchTab::onTraverseElements()
{
    // 构建示例 XML
    QDomDocument doc;
    QDomElement root = doc.createElement("公司");
    doc.appendChild(root);

    QDomElement dept = doc.createElement("部门");
    dept.setAttribute("名称", "研发部");
    QDomElement emp1 = doc.createElement("员工");
    emp1.appendChild(doc.createTextNode("张三"));
    QDomElement emp2 = doc.createElement("员工");
    emp2.appendChild(doc.createTextNode("李四"));
    dept.appendChild(emp1);
    dept.appendChild(emp2);
    root.appendChild(dept);

    // 递归遍历 DOM 树
    QString result;
    result += "===== DOM 树遍历 =====\n\n";

    // 手动递归遍历
    result += "-- 递归遍历所有节点 --\n";
    QDomNode node = doc.firstChild();
    while (!node.isNull()) {
        result += traverseNode(node, 0);
        node = node.nextSibling();
    }

    m_display->setPlainText(result);
}

// 注意：静态辅助函数，放在命名空间中
static QString traverseNode(QDomNode node, int depth)
{
    QString indent(depth * 2, ' ');
    QString result;

    if (node.isElement()) {
        QDomElement elem = node.toElement();
        result += indent + QString("元素: <%1>").arg(elem.tagName());
        if (elem.hasAttributes()) {
            QDomNamedNodeMap attrs = elem.attributes();
            QStringList attrList;
            for (int i = 0; i < attrs.count(); ++i) {
                QDomNode attr = attrs.item(i);
                attrList << QString("%1=\"%2\"").arg(attr.nodeName(), attr.nodeValue());
            }
            result += " [" + attrList.join(", ") + "]";
        }
        result += "\n";
    } else if (node.isText() && !node.toText().data().trimmed().isEmpty()) {
        result += indent + QString("文本: \"%1\"\n").arg(node.toText().data().trimmed());
    } else if (node.isProcessingInstruction()) {
        result += indent + QString("处理指令: %1\n").arg(node.toProcessingInstruction().data());
    }

    QDomNode child = node.firstChild();
    while (!child.isNull()) {
        result += traverseNode(child, depth + 1);
        child = child.nextSibling();
    }

    return result;
}

void XmlSearchTab::onReadAttributes()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("配置");
    root.setAttribute("版本", "2.0");
    root.setAttribute("环境", "开发");
    doc.appendChild(root);

    QDomElement db = doc.createElement("数据库");
    db.setAttribute("类型", "SQLite");
    db.setAttribute("端口", "3306");
    db.setAttribute("超时", "30");
    root.appendChild(db);

    QString result;
    result += "===== 属性读取 =====\n\n";

    // 读取根元素属性
    QDomElement rootElem = doc.documentElement();
    result += QString("根元素 <%1> 的属性：\n").arg(rootElem.tagName());
    QDomNamedNodeMap rootAttrs = rootElem.attributes();
    for (int i = 0; i < rootAttrs.count(); ++i) {
        QDomNode attr = rootAttrs.item(i);
        result += QString("  %1 = \"%2\"\n").arg(attr.nodeName(), attr.nodeValue());
    }

    result += "\n";

    // 读取子元素属性
    QDomElement dbElem = rootElem.firstChildElement("数据库");
    result += QString("子元素 <%1> 的属性：\n").arg(dbElem.tagName());
    result += QString("  类型 = \"%1\"\n").arg(dbElem.attribute("类型"));
    result += QString("  端口 = \"%1\"\n").arg(dbElem.attribute("端口"));
    result += QString("  超时 = \"%1\"\n").arg(dbElem.attribute("超时"));

    // 使用 attributeNS 命名空间（演示）
    result += QString("\nhasAttribute(\"类型\"): %1\n").arg(dbElem.hasAttribute("类型") ? "是" : "否");
    result += QString("hasAttribute(\"不存在\"): %1\n").arg(dbElem.hasAttribute("不存在") ? "是" : "否");

    m_display->setPlainText(result);
}

void XmlSearchTab::onModifyXml()
{
    // 创建并修改 XML
    QDomDocument doc;
    QDomElement root = doc.createElement("学生信息");
    doc.appendChild(root);

    QDomElement student = doc.createElement("学生");
    student.setAttribute("学号", "20230001");
    student.setAttribute("班级", "一班");

    QDomElement name = doc.createElement("姓名");
    name.appendChild(doc.createTextNode("王小明"));
    student.appendChild(name);

    QDomElement score = doc.createElement("成绩");
    score.appendChild(doc.createTextNode("85"));
    student.appendChild(score);

    root.appendChild(student);

    QString result;
    result += "===== XML 修改演示 =====\n\n";
    result += "修改前：\n" + doc.toString(2) + "\n";

    // 修改操作
    student.setAttribute("班级", "二班");  // 修改属性
    QDomElement scoreElem = student.firstChildElement("成绩");
    scoreElem.firstChild().setNodeValue("95");  // 修改文本

    // 添加新元素
    QDomElement level = doc.createElement("等级");
    level.appendChild(doc.createTextNode("优秀"));
    student.appendChild(level);

    // 删除元素（移除姓名后添加新姓名）
    QDomElement oldName = student.firstChildElement("姓名");
    student.removeChild(oldName);
    QDomElement newName = doc.createElement("姓名");
    newName.appendChild(doc.createTextNode("李小华"));
    student.insertBefore(newName, student.firstChild());

    result += "修改后：\n" + doc.toString(2);
    m_display->setPlainText(result);
}

// ============================================================================
// Tab5: JSON vs XML 对比
// ============================================================================

JsonXmlCompareTab::JsonXmlCompareTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(new QLabel("数据条目数："));
    m_dataCountSpin = new QSpinBox();
    m_dataCountSpin->setRange(10, 10000);
    m_dataCountSpin->setValue(100);
    m_dataCountSpin->setSingleStep(100);
    ctrlLayout->addWidget(m_dataCountSpin);

    QPushButton *btnGenerate = new QPushButton("生成两种格式");
    QPushButton *btnBench = new QPushButton("性能对比");
    QPushButton *btnSummary = new QPushButton("适用场景说明");
    ctrlLayout->addWidget(btnGenerate);
    ctrlLayout->addWidget(btnBench);
    ctrlLayout->addWidget(btnSummary);
    layout->addLayout(ctrlLayout);

    QHBoxLayout *compareLayout = new QHBoxLayout();
    compareLayout->addWidget(new QLabel("JSON："));
    m_jsonDisplay = new QTextEdit();
    m_jsonDisplay->setReadOnly(true);
    m_jsonDisplay->setFont(QFont("Courier", 9));
    compareLayout->addWidget(m_jsonDisplay);

    compareLayout->addWidget(new QLabel("XML："));
    m_xmlDisplay = new QTextEdit();
    m_xmlDisplay->setReadOnly(true);
    m_xmlDisplay->setFont(QFont("Courier", 9));
    compareLayout->addWidget(m_xmlDisplay);
    layout->addLayout(compareLayout);

    m_resultDisplay = new QTextEdit();
    m_resultDisplay->setReadOnly(true);
    m_resultDisplay->setFont(QFont("Courier", 10));
    layout->addWidget(m_resultDisplay);

    connect(btnGenerate, &QPushButton::clicked, this, &JsonXmlCompareTab::onGenerateBoth);
    connect(btnBench, &QPushButton::clicked, this, &JsonXmlCompareTab::onBenchmark);
    connect(btnSummary, &QPushButton::clicked, this, &JsonXmlCompareTab::onShowSummary);
}

void JsonXmlCompareTab::onGenerateBoth()
{
    int count = m_dataCountSpin->value();

    // 生成 JSON
    QJsonArray jsonArr;
    for (int i = 0; i < count; ++i) {
        QJsonObject item;
        item["编号"] = i + 1;
        item["姓名"] = QString("用户%1").arg(i + 1);
        item["年龄"] = 20 + (i % 30);
        item["城市"] = QString("城市%1").arg(i % 10);
        jsonArr.append(item);
    }
    QJsonDocument jsonDoc(jsonArr);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);

    // 生成 XML
    QDomDocument xmlDoc;
    QDomProcessingInstruction pi = xmlDoc.createProcessingInstruction(
        "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    xmlDoc.appendChild(pi);
    QDomElement root = xmlDoc.createElement("用户列表");
    xmlDoc.appendChild(root);

    for (int i = 0; i < count; ++i) {
        QDomElement user = xmlDoc.createElement("用户");
        user.setAttribute("编号", QString::number(i + 1));

        QDomElement name = xmlDoc.createElement("姓名");
        name.appendChild(xmlDoc.createTextNode(QString("用户%1").arg(i + 1)));
        user.appendChild(name);

        QDomElement age = xmlDoc.createElement("年龄");
        age.appendChild(xmlDoc.createTextNode(QString::number(20 + (i % 30))));
        user.appendChild(age);

        QDomElement city = xmlDoc.createElement("城市");
        city.appendChild(xmlDoc.createTextNode(QString("城市%1").arg(i % 10)));
        user.appendChild(city);

        root.appendChild(user);
    }
    QByteArray xmlData = xmlDoc.toByteArray(2);

    // 显示（截断过大数据）
    QString jsonStr = QString::fromUtf8(jsonData);
    QString xmlStr = QString::fromUtf8(xmlData);
    if (jsonStr.length() > 2000) jsonStr = jsonStr.left(2000) + "\n... (已截断)";
    if (xmlStr.length() > 2000) xmlStr = xmlStr.left(2000) + "\n... (已截断)";

    m_jsonDisplay->setPlainText(jsonStr);
    m_xmlDisplay->setPlainText(xmlStr);

    m_resultDisplay->setPlainText(QString(
        "===== 数据量对比 =====\n\n"
        "条目数：%1\n"
        "JSON 大小：%2 字节\n"
        "XML 大小：%3 字节\n"
        "XML/JSON 比例：%4")
        .arg(count)
        .arg(jsonData.size())
        .arg(xmlData.size())
        .arg(QString::number(static_cast<double>(xmlData.size()) / jsonData.size(), 'f', 2)));
}

void JsonXmlCompareTab::onBenchmark()
{
    int count = m_dataCountSpin->value();

    // 预生成数据
    QJsonArray jsonArr;
    for (int i = 0; i < count; ++i) {
        QJsonObject item;
        item["id"] = i;
        item["name"] = QString("item_%1").arg(i);
        item["value"] = i * 1.5;
        jsonArr.append(item);
    }

    // JSON 序列化/反序列化性能
    QElapsedTimer timer;

    timer.start();
    QByteArray jsonData;
    for (int r = 0; r < 100; ++r) {
        jsonData = QJsonDocument(jsonArr).toJson(QJsonDocument::Compact);
    }
    qint64 jsonSerMs = timer.elapsed();

    timer.start();
    for (int r = 0; r < 100; ++r) {
        QJsonDocument::fromJson(jsonData);
    }
    qint64 jsonDeserMs = timer.elapsed();

    // XML 序列化/反序列化性能
    QDomDocument xmlDoc;
    QDomElement root = xmlDoc.createElement("items");
    xmlDoc.appendChild(root);
    for (int i = 0; i < count; ++i) {
        QDomElement item = xmlDoc.createElement("item");
        item.setAttribute("id", QString::number(i));
        QDomElement name = xmlDoc.createElement("name");
        name.appendChild(xmlDoc.createTextNode(QString("item_%1").arg(i)));
        item.appendChild(name);
        QDomElement value = xmlDoc.createElement("value");
        value.appendChild(xmlDoc.createTextNode(QString::number(i * 1.5)));
        item.appendChild(value);
        root.appendChild(item);
    }

    timer.start();
    QByteArray xmlData;
    for (int r = 0; r < 100; ++r) {
        xmlData = xmlDoc.toByteArray();
    }
    qint64 xmlSerMs = timer.elapsed();

    timer.start();
    for (int r = 0; r < 100; ++r) {
        QDomDocument tmp;
        tmp.setContent(xmlData);
    }
    qint64 xmlDeserMs = timer.elapsed();

    QString result;
    result += QString("===== 性能对比（%1 条数据 x 100 次）=====\n\n").arg(count);
    result += QString("JSON 序列化：%1 ms\n").arg(jsonSerMs);
    result += QString("JSON 反序列化：%1 ms\n").arg(jsonDeserMs);
    result += QString("XML 序列化：%1 ms\n").arg(xmlSerMs);
    result += QString("XML 反序列化：%1 ms\n\n").arg(xmlDeserMs);
    result += QString("序列化 XML/JSON 比例：%1x\n").arg(
        QString::number(static_cast<double>(xmlSerMs) / qMax(jsonSerMs, qint64(1)), 'f', 1));
    result += QString("反序列化 XML/JSON 比例：%1x\n").arg(
        QString::number(static_cast<double>(xmlDeserMs) / qMax(jsonDeserMs, qint64(1)), 'f', 1));

    m_resultDisplay->setPlainText(result);
}

void JsonXmlCompareTab::onShowSummary()
{
    QString summary;
    summary += "===== JSON vs XML 适用场景 =====\n\n";
    summary += "【JSON 优势】\n";
    summary += "  - 数据体积小，传输效率高\n";
    summary += "  - 解析速度快\n";
    summary += "  - 与 JavaScript 天然兼容\n";
    summary += "  - 适合 REST API、Web 前后端通信\n";
    summary += "  - 支持数组和对象的无缝嵌套\n";
    summary += "  - Qt 中 QJsonObject/QJsonArray 使用方便\n\n";
    summary += "【XML 优势】\n";
    summary += "  - 支持命名空间和属性\n";
    summary += "  - 支持注释、处理指令\n";
    summary += "  - 有成熟的 Schema 验证（XSD）\n";
    summary += "  - 支持样式表转换（XSLT）\n";
    summary += "  - 适合配置文件、文档存储\n";
    summary += "  - Qt 中 QDomDocument 支持完整的 DOM 操作\n\n";
    summary += "【选择建议】\n";
    summary += "  - 网络通信/数据交换 → JSON\n";
    summary += "  - 配置文件/文档描述 → XML\n";
    summary += "  - Qt 应用配置 → QSettings 或 JSON\n";
    summary += "  - 需要文档验证 → XML + XSD\n";
    summary += "  - 简单的键值存储 → JSON\n";

    m_resultDisplay->setPlainText(summary);
}

// ============================================================================
// MainWindow 主窗口
// ============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Demo 18 - JSON/XML 数据处理");
    resize(900, 700);

    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // 添加五个标签页
    m_tabWidget->addTab(new JsonReadWriteTab(), "JSON 读写");
    m_tabWidget->addTab(new JsonAdvancedTab(), "JSON 高级");
    m_tabWidget->addTab(new XmlReadWriteTab(), "XML 读写");
    m_tabWidget->addTab(new XmlSearchTab(), "XML 搜索");
    m_tabWidget->addTab(new JsonXmlCompareTab(), "JSON vs XML 对比");
}
