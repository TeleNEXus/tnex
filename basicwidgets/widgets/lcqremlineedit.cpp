﻿#include "lcqremlineedit.h"
#include <QDebug>
#include <QKeyEvent>
#include <qnamespace.h>
#include <QKeyEvent>
//==============================================================================
LCQRemLineEdit::CReadListener::CReadListener(LCQRemLineEdit& _lineEdit) : 
  mLineEdit(_lineEdit), mFlagActive(false)
{

}

void LCQRemLineEdit::CReadListener::dataIsRead(
    QSharedPointer<QByteArray> _data, LERemoteDataStatus _status)
{
  if(mFlagActive)
  {
    if(_status != LERemoteDataStatus::DS_OK)
    {
      QString str = "Undef";
      mLineEdit.mFormatter.data()->undefState(str);
      mLineEdit.setText(str);
      mLineEdit.setEnabled(false);
      return;
      }
    mLineEdit.setEnabled(true);
    mLineEdit.setText(mLineEdit.mFormatter.data()->toString(*_data));
  }
}


LCQRemLineEdit::CWriteListener::CWriteListener(LCQRemLineEdit& _lineEdit) : 
  mLineEdit(_lineEdit)
{

}

void LCQRemLineEdit::CWriteListener::dataIsWrite(LERemoteDataStatus _status)
{
  Q_UNUSED(_status);
}

//==============================================================================
LCQRemLineEdit::LCQRemLineEdit(
    const QString& _dataName,
    QSharedPointer<LIRemoteDataSource> _dataSource,
    QSharedPointer<LIDataFormatter> _formatter,
    QWidget* _parent) :  
  QLineEdit(_parent),
  mFormatter(_formatter)
{
  QString str = "Undef";
  mFormatter.data()->undefState(str);
  setText(str);
  setValidator(_formatter->validator());
  setEnabled(false);

  mReadListener = QSharedPointer<CReadListener>(new CReadListener(*this));
  mDataReader = _dataSource->createReader(_dataName, mReadListener);

  mWriteListener = QSharedPointer<CWriteListener>(new CWriteListener(*this));
  mDataWriter = _dataSource->createWriter(_dataName, mWriteListener);
}

//------------------------------------------------------------------------------
LCQRemLineEdit::LCQRemLineEdit(
    const QString& _dataNameRead,
    const QString& _dataNameWrite,
    QSharedPointer<LIRemoteDataSource> _dataSource,
    QSharedPointer<LIDataFormatter> _formatter,
    QWidget* _parent) :  
  QLineEdit(_parent),
  mFormatter(_formatter)
{
  QString str = "Undef";
  mFormatter.data()->undefState(str);
  setText(str);
  setValidator(_formatter->validator());
  setEnabled(false);

  mReadListener = QSharedPointer<CReadListener>(new CReadListener(*this));
  mDataReader = _dataSource->createReader(_dataNameRead, mReadListener);

  mWriteListener = QSharedPointer<CWriteListener>(new CWriteListener(*this));
  mDataWriter = _dataSource->createWriter(_dataNameWrite, mWriteListener);
}

//------------------------------------------------------------------------------
LCQRemLineEdit::~LCQRemLineEdit()
{

}

void LCQRemLineEdit::setActive(bool _flag)
{
  if(_flag)
  {
    mDataReader->connectToSource();
    mReadListener->setActive(true);
  }else
  {
    setEnabled(false);
    mDataReader->disconnectFromSource();
    mReadListener->setActive(false);
  }
}

//------------------------------------------------------------------------------keyPressEvent
void LCQRemLineEdit::keyPressEvent(QKeyEvent *_ev)
{
  if((_ev->key() == Qt::Key_Enter) ||(_ev->key() == Qt::Key_Return))
  {
    mDataWriter->writeRequest(mFormatter->toBytes(text()));
    mDataReader->readRequest();
    mReadListener->setActive(true);
  }
  else
  {
    mReadListener->setActive(false);
  }
  QLineEdit::keyPressEvent(_ev);
}

void LCQRemLineEdit::focusInEvent(QFocusEvent *_event)
{
  mReadListener->setActive(false);
  QLineEdit::focusInEvent(_event);
}

void LCQRemLineEdit::focusOutEvent(QFocusEvent *_event)
{
  mReadListener->setActive(true);
  mDataReader->readRequest();
  QLineEdit::focusOutEvent(_event);
}

//------------------------------------------------------------------------------event
bool LCQRemLineEdit::event(QEvent *_event)
{
  switch(_event->type())
  {
  case QEvent::Type::Show:
    setActive(true);
    return false;

  case QEvent::Type::Hide:
    setActive(false);
    return false;

  case QEvent::KeyPress:
    //Очиска фокуса видета при нажатии клавиши Escape.
    {
      QKeyEvent* e = dynamic_cast<QKeyEvent*>(_event);
      if(e)
      {
        if( e->key() == Qt::Key_Escape)
        {
          clearFocus();
          return false;
        }
      }
    }
    break;

  default:
    break;
  }
  return QLineEdit::event(_event);
}

