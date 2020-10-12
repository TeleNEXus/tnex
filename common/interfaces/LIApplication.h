#ifndef LIAPPLICATION_H
#define LIAPPLICATION_H

#include <QSharedPointer>

class QString;
class QDir;

class LIXmlRemoteDataSourceBuilder;
class LIXmlLayoutBuilder;
class LIXmlWidgetBuilder;
class QDomElement;

#include "LIRemoteDataSource.h"

class LIApplication
{
public:
    LIApplication(){}
    /*
     * Возвращает путь к каталогу проекта.
     */
    virtual QString getProjectPath() const = 0;
    /*
     * Возвращает папку главного файла Xml.
     */
    virtual QDir getProjectDir() const = 0;
    /*
     * Возвращает интеллектуальный указатель на источник данных
     */
    virtual QSharedPointer<LIXmlRemoteDataSourceBuilder> getDataSourceBuilder(const QString& _name) const = 0;

    virtual QSharedPointer<LIRemoteDataSource> getDataSource(const QString& _name) const = 0;
    /*
     * Возвращает интеллектуальный указатель на построитель компоновцика объектов.
     */
    virtual QSharedPointer<LIXmlLayoutBuilder> getLayoutBuilder(const QString& _name) const = 0;
    /*
     * Возвращает интеллектуальный указатель на построитель визуального объекта.
     */
    virtual QSharedPointer<LIXmlWidgetBuilder> getWidgetBuilder(const QString& _name) const = 0;
    /*
     * Производит поиск и загрузку xml файла в элемент dom используя
     * относительный путь к файлу.
     */
    virtual QDomElement loadDomElement(const QString& _fileName) = 0;
};

#endif // LIAPPLICATION_H
