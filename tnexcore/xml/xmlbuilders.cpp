/* 
 * TeleNEXus is a simple SCADA programm
 *
 * Copyright (C) 2020 Sergey S. Kuzmenko
 *
 * This file is part of TeleNEXus.
 *
 * TeleNEXus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TeleNEXus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TeleNEXus.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "xmlbuilders.h"
#include "applicationinterface.h"
#include "LIXmlWidgetBuilder.h"
#include "LIXmlLayoutBuilder.h"
#include "LIXmlRemoteDataSourceBuilder.h"

#include "basicwidgetbuilders.h"
#include "basiclayoutbuilders.h"
#include "lcxmllocalsourcebuilder.h"

#include <QLibrary>
/* #include <QDebug> */
#include <QFile>

//==============================================================================
using TLibAccessFunc = void* (*)();

static const struct
{
  QString widgetBuilders  = "WIDGETBUILDERS";
  QString layoutBuilders  = "LAYOUTBUILDERS";
  QString sourceBuilders  = "SOURCEBUILDERS";
}__slRootTags;

static const struct
{
  QString Localsources = "Localsources";
}__slTags;

static const struct
{
  QString file    = "file";
  QString lib     = "lib";
  QString handler = "handler";
}__slAttributes;

enum class EAddResult
{
  Added,
  Replaced
};

#define __smMessageHeader "Builders loader:"
#define __smMessage(msg) CApplicationInterface::getInstance().message(msg)


//==============================================================================
template <typename T> QSharedPointer<T> findBuilder(
    const QString& _id, QMap<QString,QSharedPointer<T>> _map)
{
    auto it = _map.find(_id);
    if(it != _map.end())
    {
        return it.value();
    }
    return QSharedPointer<T>();
}

template <typename T> void Upload(
    QMap<QString, QSharedPointer<T>>& _map,
    const QString& _rootTag,
    const QDomElement& _documentElement, 
    const QStringList& _libPaths)
{
  auto adder = [&_map](const QString& _id, void* _builder)
  {
    EAddResult ret = (_map.contains(_id)) ? (EAddResult::Replaced) : (EAddResult::Added); 
    _map.insert(_id, QSharedPointer<T>(static_cast<T*>(_builder)));
    return ret;
  };
  load( _rootTag, _documentElement,  _libPaths, adder);
}

//==============================================================================
static void load(
    const QString& _rootTag,
    const QDomElement& _documentElement, 
    const QStringList& _libPaths,
    std::function<EAddResult(const QString& _name, void* _builder)> _adder);

static void loadBuilders(
    const QString& _rootTag, 
    const QDomElement& _element, 
    const QStringList& _libPaths,
    std::function<EAddResult(const QString& _name, void* _builder)> _adder
    );



//==============================================================================
namespace builders
{

namespace widgets
{ QMap<QString, QSharedPointer<LIXmlWidgetBuilder>> __slBuilders;}

namespace layouts
{ QMap<QString, QSharedPointer<LIXmlLayoutBuilder>> __slBuilders; }

namespace sources
{ QMap<QString, QSharedPointer<LIXmlRemoteDataSourceBuilder>> __slBuilders; }

//------------------------------------------------------------------------------
class CInitMaps
{
private:
  CInitMaps()
  {
    widgets::__slBuilders.unite(basicwidgetbuilders::getBuilders());
    layouts::__slBuilders.unite(basiclayoutbuilders::getBuilders());
    sources::__slBuilders.insert(__slTags.Localsources, 
        QSharedPointer<LIXmlRemoteDataSourceBuilder>(
          new LCXmlLocalSourceBuilder()));
  }
  CInitMaps(const CInitMaps&) = delete;
  CInitMaps& operator=(const CInitMaps&) = delete;
  static CInitMaps instance;
};
CInitMaps CInitMaps::instance;

//------------------------------------------------------------------------------
namespace widgets
{
//------------------------------------------------------------------------------
void upload( const QDomElement& _documentElement, const QStringList& _libPaths)
{
  Upload(
      __slBuilders, 
      __slRootTags.widgetBuilders, 
      _documentElement, 
      _libPaths);
}

//------------------------------------------------------------------------------
QSharedPointer<LIXmlWidgetBuilder> getBuilder(const QString _id)
{
  return findBuilder(_id, __slBuilders);
}
} /* namespace widget */ 

//------------------------------------------------------------------------------
namespace layouts
{
//------------------------------------------------------------------------------
void upload( const QDomElement& _documentElement, const QStringList& _libPaths)
{
  Upload(__slBuilders, __slRootTags.layoutBuilders, _documentElement, _libPaths);
}
//------------------------------------------------------------------------------
QSharedPointer<LIXmlLayoutBuilder> getBuilder(const QString _id)
{
  return findBuilder(_id, __slBuilders);
}
} /* namespace layouts */

//------------------------------------------------------------------------------
namespace sources 
{

//------------------------------------------------------------------------------
void upload( const QDomElement& _documentElement, const QStringList& _libPaths)
{
  Upload(
      __slBuilders, 
      __slRootTags.sourceBuilders, 
      _documentElement, 
      _libPaths);
}

//------------------------------------------------------------------------------
QSharedPointer<LIXmlRemoteDataSourceBuilder> getBuilder(const QString _id)
{
  return findBuilder(_id, __slBuilders);
}

} /* namespace sources*/


} /* namespace xmlbuilders*/



//==============================================================================
static void load(
    const QString& _rootTag,
    const QDomElement& _documentElement, 
    const QStringList& _libPaths,
    std::function<EAddResult(const QString& _name, void* _builder)> _adder)
{

  auto element = _documentElement.firstChildElement(_rootTag);
  if(element.isNull()) 
  {
    __smMessage(
          QString("%1 "
            "project root element has no element with tag '%1'")
          .arg(__smMessageHeader));
    return;
  }

  QString fileName = element.attribute(__slAttributes.file);

  if(fileName.isNull())
  {
    loadBuilders(_rootTag, element, _libPaths, _adder);
    return;
  }

  QFile file(fileName);
  QDomDocument domDoc;
  QString errorStr;
  int errorLine;
  int errorColumn;

  if(!domDoc.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
  {
    __smMessage(
      QString(
          "%1 tag '%2' load parse file '%3' error at line:%4 column:%5 msg: '%6'")
      .arg(__smMessageHeader)
      .arg(_rootTag)
      .arg(fileName)
      .arg(errorLine)
      .arg(errorColumn)
      .arg(errorStr));
    return;
  }
  
    __smMessage(
      QString("%1 tag '%2' parce file '%3'")
      .arg(__smMessageHeader)
      .arg(_rootTag)
      .arg(fileName));

  QDomElement rootElement = domDoc.documentElement();

  if(rootElement.tagName() != _rootTag)
  {
    __smMessage(
        QString("%1 file '%2' wrong root element tag name '%3'")
        .arg(__smMessageHeader)
        .arg(fileName)
        .arg(_rootTag));
    return;
  }
  loadBuilders(_rootTag, rootElement, _libPaths, _adder);
}

//==============================================================================
static void loadBuilders(
    const QString& _rootTag, 
    const QDomElement& _element, 
    const QStringList& _libPaths,
    std::function<EAddResult(const QString& _name, void* _builder)> _adder)
{

  QDomNode node = _element.firstChild();

  while(!node.isNull())
  {
    if(!node.isElement())
    {
      node = node.nextSibling();
      continue;
    }

    QDomElement el = node.toElement();

    QString libhandler = el.attribute(__slAttributes.handler);
    QString libfilename  = el.attribute(__slAttributes.lib);

    if(libhandler.isNull() || libfilename.isNull())
    {
      node = node.nextSibling();
      continue;
    }


    QLibrary lib;

    //find libraries
    for(int i = 0; i < _libPaths.size(); i++)
    {

      lib.setFileName(_libPaths[i] + "/" + libfilename);

      if(lib.isLoaded())
      {
        __smMessage(
            QString("%1 tag '%2' library '%3' is already loaded")
          .arg(__smMessageHeader)
          .arg(_rootTag)
          .arg(lib.fileName()));
        break;
      }

      if(lib.load())
      {
        __smMessage(
        QString("%1 tag '%2' library '%3' is loaded")
          .arg(__smMessageHeader)
          .arg(_rootTag)
          .arg(lib.fileName()));
        break;
      }
    }

    if(!lib.isLoaded())
    {
      __smMessage(
          QString("%1 tag '%2' can't load library '%3' to load source builder '%4'")
          .arg(__smMessageHeader)
          .arg(_rootTag)
          .arg(libfilename)
          .arg(el.tagName()));

      node = node.nextSibling();
      continue;
    }

    //load builders
    TLibAccessFunc func = (TLibAccessFunc)lib.resolve(libhandler.toStdString().c_str());

    if(!func)
    {
      __smMessage(
          QString("%1 "
            "tag '%2' can't resolve access func '%3' in library '%4' "
            "to load remote source builder '%5'")
          .arg(__smMessageHeader)
          .arg(_rootTag)
          .arg(libhandler)
          .arg(lib.fileName())
          .arg(el.tagName()));

      node = node.nextSibling();
      continue;
    }

    switch(_adder(el.tagName(), func()))
    {
    case EAddResult::Added:
      __smMessage(
          QString("%1 tag '%2' builder '%3' "
            "with access func '%4' was added")
          .arg(__smMessageHeader)
          .arg(_rootTag)
          .arg(el.tagName())
          .arg(libhandler));
      break;

    case EAddResult::Replaced:
      __smMessage(
          QString("%1 tag '%2' builder '%3' "
            "with access func '%4' was replaced")
          .arg(__smMessageHeader)
          .arg(_rootTag).
          arg(el.tagName()).
          arg(libhandler));
      break;
    }
    node = node.nextSibling();
  }
}
