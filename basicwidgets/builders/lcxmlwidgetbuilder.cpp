#include "lcxmlwidgetbuilder.h"
#include "LIApplication.h"
#include "LIXmlLayoutBuilder.h"
#include <QWidget>
#include <QDomElement>
#include <QFile>
#include <QDebug>

//------------------------------------------------------------------------------
static const struct
{
    QString layout = "layout";
} __tagsName;

//------------------------------------------------------------------------------
static const struct
{
    QString file = "file";
    QString type  = "type";
} __attrName;

//==============================================================================
LCXmlWidgetBuilder::LCXmlWidgetBuilder()
{

}

//------------------------------------------------------------------------------
LCXmlWidgetBuilder::~LCXmlWidgetBuilder()
{

}
//------------------------------------------------------------------------------
static QWidget* buildWidget(
        const QDomElement& _element, 
        const LIApplication& _app);

//------------------------------------------------------------------------------
QWidget* LCXmlWidgetBuilder::build( const QDomElement& _element, 
                                    const LIApplication& _app)
{

    QString attr;

    attr = _element.attribute(__attrName.file);

    if(!attr.isNull())
    {
        QDomElement el = _app.getDomDocument(attr).documentElement();
        if(!el.isNull())
        {
            if(el.tagName() == _element.tagName()) return buildWidget(el, _app);
        } 
    }

    return buildWidget(_element, _app);
}

//------------------------------------------------------------------------------
static QWidget* buildWidget(
        const QDomElement& _element, 
        const LIApplication& _app)
{
    QWidget* widget = new QWidget;

    QDomNodeList nodes = _element.childNodes();
    for(int i = 0; i < nodes.size(); i++)
    {
         QDomElement el = nodes.at(i).toElement();
         if(!el.isNull())
         {
                    qDebug() << "Widget set Layout 3 " << el.tagName();
             auto builder = _app.getLayoutBuilder(el.tagName());
             if(!builder.isNull())
             {
                    qDebug() << "Widget set Layout 4 " << el.tagName();
                QLayout* layout = (*builder).build(el, _app);
                if(layout)
                {
                    widget->setLayout(layout);
                    qDebug() << "Widget set Layout 5 " << el.tagName();
                    break;
                }
             }                 
         }
        
    }
    return widget;
}

