QT -= gui
QT += network
QT += serialport
QT += serialbus
QT += xml

TEMPLATE = lib
DEFINES += MBSOURCE_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=                              \
    ../../common                            \
    ../../common/interfaces                 \
    ../../common/interfaces/remotedata      \
    ../../common/interfaces/xmlbuilders     \
    ../                                     \

HEADERS += \
    ../../common/interfaces/LIApplication.h \
    ../../common/interfaces/remotedata/LIRemoteDataReadListener.h \
    ../../common/interfaces/remotedata/LIRemoteDataReader.h \
    ../../common/interfaces/remotedata/LIRemoteDataSource.h \
    ../../common/interfaces/remotedata/LIRemoteDataWriteListner.h \
    ../../common/interfaces/remotedata/LIRemoteDataWriter.h \
    ../../common/interfaces/xmlbuilders/LIXmlRemoteDataSourceBuilder.h \
    ../../common/lqextendevent.h \
    ../lmodbusdefs.h        \
    ../lqmodbusdatareader.h \
    ../lqmodbusdatasource.h \
    ../lqmodbusdatawriter.h \
    ../lqmodbusmasterbase.h \
    ../lqmodbusmasterrtu.h  \
    ../lqmodbusmastertcp.h  \
    lcxmlmodbussourcebuilder.h \
    mbsource_global.h \
    mbsource.h

SOURCES += \
    ../lqmodbusdatareader.cpp \
    ../lqmodbusdatasource.cpp \
    ../lqmodbusdatawriter.cpp \
    ../lqmodbusmasterbase.cpp \
    ../lqmodbusmasterrtu.cpp \
    ../lqmodbusmastertcp.cpp \
    lcxmlmodbussourcebuilder.cpp \
    mbsource.cpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
