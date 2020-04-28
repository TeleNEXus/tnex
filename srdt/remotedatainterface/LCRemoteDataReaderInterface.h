#ifndef LCREMOTEDATAREADERINTERFACE_H
#define LCREMOTEDATAREADERINTERFACE_H

#include <QWeakPointer>

class LCRemoteDataSourceInterface;
class LCRemoteDataReadListnerInterface;

class LCRemoteDataReaderInterface
{
public:
    LCRemoteDataReaderInterface(){}
    ~LCRemoteDataReaderInterface(){}
    virtual void setDataName(const QString& _dataName) = 0;
    virtual void setDataReadListener(QWeakPointer<LCRemoteDataReadListnerInterface> _listener) = 0;
    virtual void readRequest() = 0;
    virtual void connectToSource() = 0;
    virtual void disconnectFromSource() = 0;
};

#endif // LCREMOTEDATAREADERINTERFACE_H
