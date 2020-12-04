#include "lcqreadfromsourcereq.h"
#include <QCoreApplication>
#include <QDebug>

//==============================================================================CEventBase
__LQ_EXTENDED_QEVENT_IMPLEMENTATION(LCQReadFromSourceReq::CEventBase);

LCQReadFromSourceReq::CEventBase::CEventBase() : 
  QEvent(__LQ_EXTENDED_QEVENT_REGISTERED)
{
}

//==============================================================================CEventRead
LCQReadFromSourceReq::CEventRead::CEventRead()
{
}

//------------------------------------------------------------------------------
void LCQReadFromSourceReq::CEventRead::handle(LCQReadFromSourceReq* _sender)
{
  _sender->mRetData = _sender->mDataId;
  _sender->mWaitCond.wakeOne();

}

static void requestDeleter(LCQReadFromSourceReq* _req)
{
  qDebug() << "Read Request Deleter";
  _req->deleteLater();
}
//==============================================================================
QSharedPointer<LCQReadFromSourceReq> LCQReadFromSourceReq::create(
    const QString& _dataId,
    QThread* _thread)
{
  LCQReadFromSourceReq* req = new LCQReadFromSourceReq(_dataId);
  req->moveToThread(_thread);
  return QSharedPointer<LCQReadFromSourceReq>(req, requestDeleter);
}

LCQReadFromSourceReq::LCQReadFromSourceReq(const QString& _dataId) :
  QObject(nullptr),
  mDataId(_dataId)
{
}

//------------------------------------------------------------------------------
LCQReadFromSourceReq::~LCQReadFromSourceReq()
{
}

//------------------------------------------------------------------------------
QString LCQReadFromSourceReq::getData()
{
  mMutexEvent.lock();
  QCoreApplication::postEvent(this, new CEventRead());
  mWaitCond.wait(&mMutexEvent);
  mMutexEvent.unlock();
  return mRetData;
}

//------------------------------------------------------------------------------
void LCQReadFromSourceReq::customEvent(QEvent* _event)
{
  mMutexEvent.lock();
  mMutexEvent.unlock();

  if(_event->type() == CEventBase::msExtendedEventType)
  {
    CEventBase* e = dynamic_cast<CEventBase*>(_event);
    if(e == nullptr)
    {
      return;
    }
    e->handle(this);
  }
}

