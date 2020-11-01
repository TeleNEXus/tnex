#ifndef LQMODBUSDATAREADER_H
#define LQMODBUSDATAREADER_H

#include "lqextendevent.h"
#include "LIRemoteDataSource.h"
#include "LIRemoteDataReader.h"

namespace modbus {

class LQModbusDataSource;

class LQModbusDataReader : public QObject, public LIRemoteDataReader
{
    Q_OBJECT
private:

    //---------------------------------------------------------------------------------------------------CQEventDataRead
    class CQEventDataIsRead : public QEvent
    {
        __LQ_EXTENDED_QEVENT_DECLARATION
    public:
        QSharedPointer<QByteArray> mData;
        LERemoteDataStatus mStatus;
        explicit CQEventDataIsRead(const QByteArray& _data, LERemoteDataStatus _status);
        explicit CQEventDataIsRead(LERemoteDataStatus _status);
    };

    QString mDataName;
    QWeakPointer<LIRemoteDataSource> mDataSource;
    QWeakPointer<LIRemoteDataReadListener> mpReadListener;

public:

    virtual ~LQModbusDataReader();
    virtual void setDataName(const QString& _dataName) override;
    virtual void setDataReadListener(QWeakPointer<LIRemoteDataReadListener> _listener) override;
    virtual void readRequest() override;
    virtual void connectToSource() override;
    virtual void disconnectFromSource() override;
private:
    explicit LQModbusDataReader(  QObject* _parent = nullptr) = delete;
    explicit LQModbusDataReader(  QWeakPointer<LQModbusDataSource> _dataSource, 
            QObject* _parent = nullptr);
 
    virtual void customEvent(QEvent* _event) override;
    friend class LQModbusDataSource;
};

} //namespace
#endif // LQMODBUSDATAREADER_H
