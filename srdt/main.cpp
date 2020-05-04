﻿
#include <signal.h>
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QHostAddress>

#include "lqmodbusmastertcp.h"
#include "lqmodbusmasterrtu.h"
#include "lqmodbusdatasource.h"
#include <QMap>
#include <QEvent>
#include <QMutex>
#include <atomic>
#include <QByteArray>
#include <QList>
#include <QBitArray>


#include <QIntValidator>
#include <QDoubleValidator>



//using namespace std;

QThread* __gStopTh1 = nullptr;

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QTabBar>
#include <QToolBox>

#include <functional>
#include <limits>

#include "lcqremlabel.h"
#include "lcqremlineedit.h"

#include "lcstringdataformatterbool.h"

#include "lcstringdataformatteru8.h"
#include "lcstringdataformatters8.h"

#include "lcstringdataformatteru16.h"
#include "lcstringdataformatters16.h"

#include "lcstringdataformatteru32.h"
#include "lcstringdataformatters32.h"

#include "lcstringdataformatterf32.h"
#include "lqvalidatoru32.h"

#include "lcstringdataformatterhex.h"

#include "lqmodbusmastertcp.h"



#include <memory>

#include "lcxmlapplication.h"

#include <QDomElement>
#include <QDomDocument>
#include "lcxmlwidgetcreatorsmap.h"
#include "xmlwidgets/lcxmlwidget.h"
#include "xmlwidgets/lcxmllabel.h"

#include <QLibrary>

#include "testdllinterface.h"
#include "lctestdllclass.h"


class CTestDllRecurse : public LCTestDllInterface
{
public:
    CTestDllRecurse(){}
    virtual ~CTestDllRecurse(){}
    virtual QString getString() override {return "LCTestDllInterface Test Dll String";}
};

//======================================================================================================================
int main(int argc, char *argv[])
{
    QLibrary lib("D:/Dokuments/Kuzmenko/Programs/PROGRAM_PROJECTS/srdtrep/build-extwidglib-Desktop-Debug/debug/extwidglib");
    typedef QSharedPointer<LCTestDllInterface> (*TFct)();

    TFct fct = (TFct)(lib.resolve("getTestDllInterface"));

//    CTestDllRecurse tci;
    if(!fct)
    {
        qDebug()<< "No recognize function";
    }
    else
    {
        QSharedPointer<LCTestDllInterface> test = fct();
        qDebug()<< test->getString();
    }
    qDebug()<< "End programm";

//    if(lib.resolve("Extwidglib"))
//    {
//        qDebug()<< "resolve yes";
//    }
//    else
//    {
//        qDebug()<< "resolve no";
//    }

//    return 0;

    return LCXmlApplication::instance().exec(argc, argv);
}

