﻿#include "lcqremlineedit.h"
#include <QDebug>
#include <QKeyEvent>
namespace remgui
{
//======================================================================================================================
LCQRemLineEdit::CReadListener::CReadListener(LCQRemLineEdit& _lineEdit) : mLineEdit(_lineEdit), mFlagActive(false)
{

}

void LCQRemLineEdit::CReadListener::dataIsRead(QSharedPointer<QByteArray> _data, ERemoteDataStatus _status)
{
    if(mFlagActive)
    {
        if(_status != ERemoteDataStatus::DS_OK)
        {
            mLineEdit.setText(mLineEdit.mFormatter.data()->undefStateString());
            return;
        }
        mLineEdit.setEnabled(true);
        mLineEdit.setText(mLineEdit.mFormatter.data()->toString(*_data));
    }
}


LCQRemLineEdit::CWriteListener::CWriteListener(LCQRemLineEdit& _lineEdit) : mLineEdit(_lineEdit)
{

}

void LCQRemLineEdit::CWriteListener::dataIsWrite(ERemoteDataStatus _status)
{
    Q_UNUSED(_status);
}

//======================================================================================================================
LCQRemLineEdit::LCQRemLineEdit(const QString& _dataName,
                               QSharedPointer<LCRemoteDataSourceInterface> _dataSource,
                               QSharedPointer<LCDataStrFormatBase> _formatter,
                               QWidget* _parent) :  QLineEdit(_parent),
                                                    mFormatter(_formatter)
{
    setText(mFormatter.data()->undefStateString());
    setValidator(_formatter->validator());
    setEnabled(false);

    mDataReader = _dataSource->createReader();
    mDataReader->setDataName(_dataName);
    mReadListener = QSharedPointer<CReadListener>(new CReadListener(*this));
    mDataReader->setDataReadListener(mReadListener);
    mDataReader->setDataSource(_dataSource);

    mDataWriter = _dataSource->createWriter();
    mDataWriter->setDataName(_dataName);
    mWriteListener = QSharedPointer<CWriteListener>(new CWriteListener(*this));
    mDataWriter->setDataWriteListener(mWriteListener);
    mDataWriter->setDataSource(_dataSource);
}

//----------------------------------------------------------------------------------------------------------------------
LCQRemLineEdit::LCQRemLineEdit(const QString& _dataNameRead,
                               const QString& _dataNameWrite,
                               QSharedPointer<LCRemoteDataSourceInterface> _dataSource,
                               QSharedPointer<LCDataStrFormatBase> _formatter,
                               QWidget* _parent) :  QLineEdit(_parent),
                                                    mFormatter(_formatter)
{
    setText(mFormatter.data()->undefStateString());
    setValidator(_formatter->validator());
    setEnabled(false);

    mDataReader = _dataSource->createReader();
    mDataReader->setDataName(_dataNameRead);
    mReadListener = QSharedPointer<CReadListener>(new CReadListener(*this));
    mDataReader->setDataReadListener(mReadListener);

    mDataWriter = _dataSource->createWriter();
    mDataWriter->setDataName(_dataNameWrite);
    mWriteListener = QSharedPointer<CWriteListener>(new CWriteListener(*this));
    mDataWriter->setDataWriteListener(mWriteListener);
}

//----------------------------------------------------------------------------------------------------------------------
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

void LCQRemLineEdit::keyPressEvent(QKeyEvent *_ev)
{
    if((_ev->key() == Qt::Key_Enter) ||(_ev->key() == Qt::Key_Return))
    {
        mDataWriter->writeRequest(mFormatter->toBytes(text()));
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
    QLineEdit::focusOutEvent(_event);
}

//-----------------------------------------------------------------------------------------------------------------event
bool LCQRemLineEdit::event(QEvent *_event)
{
    bool ret = true;
    switch(_event->type())
    {
    case QEvent::Type::Show:
        setActive(true);
        ret = false;
        break;
    case QEvent::Type::Hide:
        setActive(false);
        ret = false;
        break;
    default:
        QLineEdit::event(_event);
        break;
    }
    return ret;
}

}
