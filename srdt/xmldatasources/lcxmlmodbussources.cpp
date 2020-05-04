#include "lcxmlmodbussources.h"

#include <QFile>
#include <QDebug>
#include <QDomElement>

#include "lqmodbusmasterbase.h"

#include "lqmodbusmasterrtu.h"
#include "lqmodbusmastertcp.h"

#include "lqmodbusdatasource.h"

#include "lcxmlapplication.h"

using namespace modbus;

using LTMastersMap = QMap<QString, QSharedPointer<LQModbusMasterBase>>;

//======================================================================================================================
LCXmlModbusSources::LCXmlModbusSources()
{

}

//----------------------------------------------------------------------------------------------------------------------
static LTMastersMap createMasters(const QDomNodeList& nodes);

//----------------------------------------------------------------------------------------------------------------------
static void createSources(const QDomNodeList& nodes,
                          LTMastersMap _masters,
                          LQDataSources& _sourcesmap);

//----------------------------------------------------------------------------------------------------------------------
LQDataSources LCXmlModbusSources::create(const QDomElement &_element)
{
    LQDataSources map;
    QFile file;
    QString xmlfilename = _element.attribute("file");

    if(xmlfilename.isNull()) return map;

    file.setFileName(LCXmlApplication::instance().getXmlMainFileWay() + xmlfilename);

    QDomDocument domDoc;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!domDoc.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << "LCXmlModbusSources: parse error at line:"
                 << errorLine << " column:" << errorColumn << " msg: " << errorStr;
        return map;
    }

    QDomElement rootElement = domDoc.documentElement();

    if(rootElement.tagName() != "modbussources")
    {
        qDebug() << "LCXmlModbusSources: wrong root element";
        return map;
    }

    QDomNodeList nodes = rootElement.elementsByTagName("master");

    if(nodes.isEmpty())
    {
        qDebug() << "LCXmlModbusSources: no elements modbus masters";
        return map;
    }

    LTMastersMap masters = createMasters(nodes);

    if(masters.isEmpty())
    {
        qDebug() << "LCXmlModbusSources: no valid modbus masters";
        return map;
    }

    nodes = rootElement.elementsByTagName("source");

    if(nodes.isEmpty())
    {
        qDebug() << "LCXmlModbusSources: no elements modbus source";
        return map;
    }

    createSources(nodes, masters, map);

    return map;
}

//----------------------------------------------------------------------------------------------------------------------
static int loadMemoryMap(LQModbusDataSource* _p_source, const QString& _filename);

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString name            = "name";
    QString master          = "master";
    QString deviceid        = "devid";
    QString updatetime      = "updatetime";
    QString memorymapfile   = "mapfile";

}__sourceAttributes;
//----------------------------------------------------------------------------------------------------------------------
static void createSources(const QDomNodeList& nodes,
                          LTMastersMap _masters,
                          LQDataSources& _sourcesmap)
{
    QString attr;
    QString attrName;

    LTMastersMap::iterator itm;

    quint32 devid = 0;
    bool boolBuff = false;

    for(int i = 0; i < nodes.size(); i++)
    {
        QDomElement el;
        qDebug() << "createSources pass0";
        el = nodes.at(i).toElement();

        attrName = el.attribute(__sourceAttributes.name);
        if(attrName.isNull()) continue;
        if(_sourcesmap.contains(attrName)) continue;

        attr = el.attribute(__sourceAttributes.master);
        if(attr.isNull()) continue;

        itm = _masters.find(attr);

        if(itm == _masters.end()) continue;

        attr = el.attribute(__sourceAttributes.deviceid);
        if(attr.isNull()) continue;

        devid = attr.toUInt(&boolBuff);

        if(!boolBuff) continue;
        if(devid > 255) continue;

        attr = el.attribute(__sourceAttributes.memorymapfile);
        if(attr.isNull()) continue;

        QSharedPointer<LQModbusDataSource> source = LQModbusDataSource::create(devid, itm.value());

        if(loadMemoryMap(source.data(), LCXmlApplication::instance().getXmlMainFileWay() + attr) != 0)
        {
            int updatetime;
            bool flag;

            _sourcesmap.insert(attrName, source);

            attr = el.attribute(__sourceAttributes.updatetime);

            updatetime = attr.toInt(&flag);

            if(flag)
            {
                source->start(QSharedPointer<QThread>(new QThread), updatetime);
            }
            else
            {
                source->start(QSharedPointer<QThread>(new QThread));
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString name        = "name";
    QString type        = "type";
}__mastersCommonAttributes;

//----------------------------------------------------------------------------------------------------------------------
static QSharedPointer<LQModbusMasterBase> createMasterRtu(const QDomElement _element);
//----------------------------------------------------------------------------------------------------------------------
static QSharedPointer<LQModbusMasterBase> createMasterTcp(const QDomElement _element);

//----------------------------------------------------------------------------------------------------------------------
static LTMastersMap createMasters(const QDomNodeList& nodes)
{
    LTMastersMap map;
    QString attrType;
    QString attrName;
    for(int i = 0; i < nodes.size(); i++)
    {
        QDomElement el;
        el = nodes.at(i).toElement();

        attrName = el.attribute(__mastersCommonAttributes.name);
        if(attrName.isNull()) continue;
        if(map.contains(attrName))continue;

        attrType = el.attribute(__mastersCommonAttributes.type);
        if(attrType.isNull()) continue;

        QSharedPointer<LQModbusMasterBase> master;
        if(attrType == "rtu")
        {
            master = createMasterRtu(el);
            if(!master.isNull()) map.insert(attrName, master);
        }
        else if(attrType == "tcp")
        {
            master = createMasterTcp(el);
            if(!master.isNull()) map.insert(attrName, master);
        }
    }

    return map;
}

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString port        = "portname";
    QString baudrate    = "baud";
    QString parity      = "parity";
    QString databits    = "databits";
    QString stopbits    = "stopbits";
    QString timeout     = "timeout";

    static QSerialPort::Parity getParity(const QString& _name)
    {
        static struct
        {
            QString noparity    = "no";
            QString evenparity  = "even";
            QString oddparity   = "odd";
            QString spaceparity = "space";
            QString markparity  = "mark";
        }parytyAttr;

        if(_name == parytyAttr.noparity)    return QSerialPort::Parity::NoParity;
        if(_name == parytyAttr.evenparity)  return QSerialPort::Parity::EvenParity;
        if(_name == parytyAttr.oddparity)   return QSerialPort::Parity::OddParity;
        if(_name == parytyAttr.spaceparity) return QSerialPort::Parity::SpaceParity;
        if(_name == parytyAttr.markparity)  return QSerialPort::Parity::MarkParity;
        return QSerialPort::Parity::UnknownParity;
    }

}__mastersRtuAttributes;


//----------------------------------------------------------------------------------------------------------------------
static QSharedPointer<LQModbusMasterBase> createMasterRtu(const QDomElement _element)
{
    QString attr;

    QString portName;
    int baud;
    QSerialPort::Parity parity;
    int dataBits;
    int stopBits;
    int timeout;

    bool boolBuff = false;

    QSharedPointer<LQModbusMasterRtu> master;

    portName = _element.attribute(__mastersRtuAttributes.port);
    if(portName.isNull()) goto LABELRET;

    attr = _element.attribute(__mastersRtuAttributes.baudrate);
    baud = attr.toInt(&boolBuff);
    if(!boolBuff) goto LABELRET;

    attr = _element.attribute(__mastersRtuAttributes.parity);
    parity = __mastersRtuAttributes.getParity(attr);
    if(parity == QSerialPort::Parity::UnknownParity) goto LABELRET;


    attr = _element.attribute(__mastersRtuAttributes.databits);
    dataBits = attr.toInt(&boolBuff);
    if(!boolBuff || (dataBits < 0)) goto LABELRET;

    attr = _element.attribute(__mastersRtuAttributes.stopbits);
    if(attr.isNull()) goto LABELRET;

    stopBits = attr.toInt(&boolBuff);
    if(!boolBuff || (stopBits < 0)) goto LABELRET;

    master = LQModbusMasterRtu::create();

    attr = _element.attribute(__mastersRtuAttributes.timeout);
    timeout = attr.toInt(&boolBuff);

    if(boolBuff)
    {
        master->connectToPort(  portName,
                                static_cast<QSerialPort::BaudRate>(baud),
                                static_cast<QSerialPort::Parity>(parity),
                                static_cast<QSerialPort::DataBits>(dataBits),
                                static_cast<QSerialPort::StopBits>(stopBits));
    }
    else
    {
        master->connectToPort(  portName,
                                static_cast<QSerialPort::BaudRate>(baud),
                                static_cast<QSerialPort::Parity>(parity),
                                static_cast<QSerialPort::DataBits>(dataBits),
                                static_cast<QSerialPort::StopBits>(stopBits),
                                timeout);
    }

LABELRET:
    return master;
}

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString url = "url";

}__mastersTcpAttributes;

//----------------------------------------------------------------------------------------------------------------------
static QSharedPointer<LQModbusMasterBase> createMasterTcp(const QDomElement _element)
{
    QSharedPointer<LQModbusMasterTcp> master;
    QString urlstr = _element.attribute(__mastersTcpAttributes.url);

    if(urlstr.isNull()) return master;

    QUrl url = QUrl::fromUserInput(urlstr);

    if(url.isValid())
    {
        master = LQModbusMasterTcp::create();
        master->connectToHost(url);

    }
    return master;
}

//----------------------------------------------------------------------------------------------------------------------
enum class EItemType
{
    COILS,
    DISCRETEINPUTS,
    INPUTREGISTERS,
    HOLDINGREGISTERS
};

//----------------------------------------------------------------------------------------------------------------------
static int addSourceDataItems(LQModbusDataSource* _p_source, const QDomNodeList& _nodes, EItemType _type);

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString coils               = "coil";
    QString discreteinputs      = "discin";
    QString inputregisters      = "inreg";
    QString holdingregisters    = "holdreg";
}__memoryMapItemElementNames;

//----------------------------------------------------------------------------------------------------------------------
static int loadMemoryMap(LQModbusDataSource* _p_source,  const QString& _filename)
{
    QFile file(_filename);

    QDomDocument domDoc;
    QDomNodeList itemnodes;
    QString errorStr;
    int errorLine;
    int errorColumn;
    int itemsCounter = 0;

    if(!domDoc.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << "LCXmlModbusSources: parse memory map file:" << _filename << " "
                 << errorLine << " column:" << errorColumn << " msg: " << errorStr;
        return -1;
    }

    QDomElement rootElement = domDoc.documentElement();

    if(rootElement.tagName() != "memorymap")
    {
        qDebug() << "LCXmlModbusSources: memory map file " <<  _filename << " wrong root element";
        return -2;
    }

    itemnodes = rootElement.elementsByTagName(__memoryMapItemElementNames.coils);
    if(!itemnodes.isEmpty())
    {
        itemsCounter += addSourceDataItems(_p_source, itemnodes, EItemType::COILS);
    }

    itemnodes = rootElement.elementsByTagName(__memoryMapItemElementNames.discreteinputs);
    if(!itemnodes.isEmpty())
    {
        itemsCounter += addSourceDataItems(_p_source, itemnodes, EItemType::DISCRETEINPUTS);
    }

    itemnodes = rootElement.elementsByTagName(__memoryMapItemElementNames.inputregisters);
    if(!itemnodes.isEmpty())
    {
        itemsCounter += addSourceDataItems(_p_source, itemnodes, EItemType::INPUTREGISTERS);
    }

    itemnodes = rootElement.elementsByTagName(__memoryMapItemElementNames.holdingregisters);
    if(!itemnodes.isEmpty())
    {
        itemsCounter += addSourceDataItems(_p_source, itemnodes, EItemType::HOLDINGREGISTERS);
    }

    return itemsCounter;
}

//----------------------------------------------------------------------------------------------------------------------
static const struct
{
    QString name = "name";
    QString addr = "addr";
    QString size = "size";
}__memoryMapItemAttributes;

//----------------------------------------------------------------------------------------------------------------------
/*
 * Возвращает количество добавленных сущностей карты памяти.
 */
static int addSourceDataItems(LQModbusDataSource* _p_source, const QDomNodeList& _nodes, EItemType _type)
{
    QDomElement element;
    QString attr;
    QString name;
    int  addr;
    int  size;
    bool boolBuff;
    int  itemCounter = 0;

    for(int i = 0; i < _nodes.size(); i++)
    {
        element = _nodes.at(i).toElement();

        name = element.attribute(__memoryMapItemAttributes.name);
        if(name.isNull()) continue;

        attr = element.attribute(__memoryMapItemAttributes.addr);
        if(attr.isNull()) continue;

        addr = attr.toInt(&boolBuff);
        if( (!boolBuff) ||
            (addr < 0)  ||
            (addr > 0x0000ffff))
        {
            continue;
        }

        attr = element.attribute(__memoryMapItemAttributes.size);
        if(attr.isNull())
        {
            size = 1;
        }
        else
        {
            size = attr.toInt(&boolBuff);
            if( (!boolBuff) ||
                (addr < 0)  ||
                (addr > 0x0000ffff))
            {
                continue;
            }
        }

        switch(_type)
        {
        case EItemType::COILS:
            _p_source->addDataItemCoils(name, addr, size);
            break;

        case EItemType::DISCRETEINPUTS:
            _p_source->addDataItemDiscreteInputs(name, addr, size);
            break;

        case EItemType::INPUTREGISTERS:
            _p_source->addDataItemInputRegs(name, addr, size);
            break;

        case EItemType::HOLDINGREGISTERS:
            _p_source->addDataItemHoldingRegs(name, addr, size);
            break;
        }
        itemCounter++;
    }
    return itemCounter;
}
