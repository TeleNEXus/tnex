#include "lcxmlwidgetbuilders.h"
#include "lcxmlapplication.h"

#include <QMap>
#include <QLibrary>
#include <QCoreApplication>

//======================================================================================================================
using LQLayoutBuildersMap = QMap<QString, QSharedPointer<LIXmlWidgetBuilder>>;

//======================================================================================================================
static LQLayoutBuildersMap __buildersMap;

//======================================================================================================================
LCXmlWidgetBuilders::LCXmlWidgetBuilders() :
        LCXmlBuildersLoader(    LCXmlApplication::mBaseTagNames.sourceBuilders,
                                LCXmlApplication::mBaseTagNames.builder,
                                LCXmlApplication::mBaseAttributeNames.file,
                                LCXmlApplication::mBaseAttributeNames.name,
                                LCXmlApplication::mBaseAttributeNames.lib,
                                LCXmlApplication::mBaseAttributeNames.libhandler)
{
}

//----------------------------------------------------------------------------------------------------------------------
LCXmlWidgetBuilders& LCXmlWidgetBuilders::instance()
{
    static LCXmlWidgetBuilders inst;
    return inst;
}

//----------------------------------------------------------------------------------------------------------------------
QSharedPointer<LIXmlWidgetBuilder> LCXmlWidgetBuilders::getBuilder(const QString _name)
{
    LQLayoutBuildersMap::iterator it = __buildersMap.find(_name);
    if(it != __buildersMap.end())
    {
        return it.value();
    }
    return QSharedPointer<LIXmlWidgetBuilder>();
}

//----------------------------------------------------------------------------------------------------------------------
bool LCXmlWidgetBuilders::add(const QString &_name, void *_builder)
{
    if(__buildersMap.find(_name) != __buildersMap.end()) return false;

    __buildersMap.insert(_name,
                         QSharedPointer<LIXmlWidgetBuilder>
                            (reinterpret_cast<LIXmlWidgetBuilder*>(_builder)));
    return true;
}
