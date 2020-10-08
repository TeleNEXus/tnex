#include "lcxmltablewidgetbuilder.h"
#include <QTableWidget>
#include <QDomElement>
#include <qtablewidget.h>
#include <QDebug>
#include "LIApplication.h"

//==============================================================================
LCXmlTableWidgetBuilder::LCXmlTableWidgetBuilder()
{

}

//------------------------------------------------------------------------------
LCXmlTableWidgetBuilder::~LCXmlTableWidgetBuilder()
{

}

//------------------------------------------------------------------------------
static const struct
{
    QString name = "name";
} __attrNames;

//------------------------------------------------------------------------------
static const struct
{
    QString row = "row";
    QString column = "col";
} __elementNames;

//------------------------------------------------------------------------------
struct SBuildData
{
    quint32 mRow = 0;
    quint32 mColumn = 0;
    QTableWidget* mpTable= nullptr;
    SBuildData() : mpTable(new QTableWidget){}
};

//------------------------------------------------------------------------------
static void createRow(
        const QDomElement &_element,
        const LIApplication& _app,
        SBuildData& _buildData);

static void createCol(
        const QDomElement &_element,
        const LIApplication& _app,
        SBuildData& _buildData);

//------------------------------------------------------------------------------
QWidget* LCXmlTableWidgetBuilder::build(const QDomElement& _element,
        const LIApplication& _app)
{
    SBuildData buildData;

    QDomNode childNode = _element.firstChild();

    while(!childNode.isNull())
    {
        if(childNode.isElement())
        {
            QDomElement element = childNode.toElement();
            if(element.tagName() == __elementNames.row)
            {
                createRow(element, _app, buildData);
            }
            else if(element.tagName() == __elementNames.column)
            {
                createCol(element, _app, buildData);

            }
        }
        //--------------------------------
        childNode = childNode.nextSibling();
    }
    return buildData.mpTable;

//    QTableWidget* table = new QTableWidget();
//    qDebug() << "------------------------------------Create QTableWidget Start";
//    table->setRowCount(5);
//    table->setColumnCount(5);
//    qDebug() << "Table widget row count     =" << table->rowCount();
//    qDebug() << "Table widget column count  =" << table->columnCount();
//    return table;
}

//------------------------------------------------------------------------------
static void createRow(
        const QDomElement &_element, 
        const LIApplication& _app, 
        SBuildData& _buildData)
{
    quint32 col = 0;
    QDomNode childNode = _element.firstChild();

    _buildData.mpTable->setRowCount(++_buildData.mRow);

    QString row_name = _element.attribute(__attrNames.name);

    if(!row_name.isNull())
    {
        _buildData.mpTable->setVerticalHeaderItem(_buildData.mRow - 1, 
                new QTableWidgetItem(row_name));
    }

    if(childNode.isNull())
    {
        return;
    }

    while(!childNode.isNull())
    {
        if(!childNode.isElement())
        {
            childNode = childNode.nextSibling();
            continue;
        }

        QDomElement nodeElement = childNode.toElement();

        QWidget* widget = nullptr;

        //Попытка получения построителя виджета.
        QSharedPointer<LIXmlWidgetBuilder> widget_builder = 
            _app.getWidgetBuilder(nodeElement.tagName());

        col++;
        if(col > _buildData.mColumn)  
        {
            _buildData.mpTable->setColumnCount(col);
            _buildData.mColumn = col;
        }

        if(!widget_builder.isNull())
        {
            widget = widget_builder->build(nodeElement, _app);
        }

        if(widget)
        {
            _buildData.mpTable->setCellWidget(
                    _buildData.mRow - 1, 
                    col - 1, 
                    widget);
        }

        //--------------------------------
        childNode = childNode.nextSibling();
    }
}

//------------------------------------------------------------------------------
static void createCol(
        const QDomElement &_element, 
        const LIApplication& _app, 
        SBuildData& _buildData)
{
    quint32 row = 0;
    QDomNode childNode = _element.firstChild();

    _buildData.mpTable->setColumnCount(++_buildData.mColumn);

    QString col_name = _element.attribute( __attrNames.name);

    if(!col_name.isNull())
    {
        _buildData.mpTable->setHorizontalHeaderItem(_buildData.mColumn -1, 
            new QTableWidgetItem(col_name));
    }

    if(childNode.isNull())
    {
        return;
    }

    while(!childNode.isNull())
    {
        if(!childNode.isElement())
        {
            childNode = childNode.nextSibling();
            continue;
        }

        QDomElement nodeElement = childNode.toElement();

        QWidget* widget = nullptr;
        //Попытка получения построителя виджета.
        QSharedPointer<LIXmlWidgetBuilder> widget_builder = 
            _app.getWidgetBuilder(nodeElement.tagName());

        //Добавление новой строки.
        row++;
        if(row > _buildData.mRow)  
        {
            _buildData.mpTable->setRowCount(row);
            _buildData.mRow = row;
        }

        if(!widget_builder.isNull())
        {
            widget = widget_builder->build(nodeElement, _app);
        }

        if(widget)
        {
            _buildData.mpTable->setCellWidget(
                    row - 1, 
                    _buildData.mColumn - 1, 
                    widget);
        }

        //--------------------------------
        childNode = childNode.nextSibling();
    }
}
