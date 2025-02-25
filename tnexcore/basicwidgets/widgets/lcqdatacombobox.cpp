﻿/* 
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
#include "lcqdatacombobox.h"
#include "LIRemoteDataReader.h"
#include "LIRemoteDataWriter.h"

#include <qglobal.h>
#include <QKeyEvent>
#include <QDebug>

//------------------------------------------------------------------------------
LCQDataComboBox::LCQDataComboBox(    
    QSharedPointer<LIRemoteDataReader>  _dataReader,
    QSharedPointer<LIDataFormatter>     _formatterRead,
    QSharedPointer<LIRemoteDataWriter>  _dataWriter,
    QSharedPointer<LIDataFormatter>     _formatterWrite,
    QWidget* _parent) :
  QComboBox(_parent),
  mDataReader(_dataReader),
  mFormatterRead(_formatterRead),
  mDataWriter(_dataWriter),
  mFormatterWrite(_formatterWrite),
  mFlagPopupOn(false)
{

  auto read_handler = 
    [this](QSharedPointer<QByteArray> _data, LIRemoteDataReader::EReadStatus _status)
    {
      using EReadStatus = LIRemoteDataReader::EReadStatus;
      switch(_status)
      {
      case EReadStatus::Valid:
        setCurrentIndex(findData( mFormatterRead->toString( *_data.data()))); 
        setEnabled(true);
        break;

      case EReadStatus::Wrong:
        setCurrentIndex(-1);
        setEnabled(true);
        break;

      case EReadStatus::Undef:
        setCurrentIndex(-1);
        setEnabled(false);
      default:
        break;
      }
    };

  mDataReader->setHandler(read_handler);

  connect(this, static_cast <void(LCQDataComboBox::*)(int)> 
      (&LCQDataComboBox::activated),
      [this](int index)
      {
        Q_UNUSED(index);
        mDataWriter->writeRequest(
            mFormatterWrite->toBytes(currentData().toString()));
      });

  setEnabled(false);
}

//------------------------------------------------------------------------------
LCQDataComboBox::~LCQDataComboBox()
{
}

//------------------------------------------------------------------------------
bool LCQDataComboBox::event(QEvent *_event)
{
  int key;
  bool ret = false;
  switch(_event->type())
  {
  case QEvent::Type::Show:
    if(!mIsVisibleFlag)
    {
      mIsVisibleFlag = true;
      mDataReader->connectToSource();
      setCurrentIndex(-1);
      ret = true;
    }
    break;

  case QEvent::Type::Hide:
    if(mIsVisibleFlag)
    {
      mIsVisibleFlag = false;
      mDataReader->disconnectFromSource();
      ret = true;
    }
    break;

  case QEvent::Type::KeyPress:
    //Очиска фокуса видета при нажатии клавиши Escape.
    key = static_cast<QKeyEvent*>(_event)->key();
    if(( key == Qt::Key_Enter) || ( key == Qt::Key_Return))
    {
      showPopup();
    } else if( static_cast<QKeyEvent*>(_event)->key() == Qt::Key_Escape)
    {
      if ( mFlagPopupOn )
      {
        hidePopup();
      }
      else
      {
        clearFocus();
      }
    }
    ret = true;
    break;

  default:
  ret = QComboBox::event(_event);
    break;
  }
  return ret;
}

//------------------------------------------------------------------------------
void LCQDataComboBox::showPopup(void)
{
  mFlagPopupOn = true;
  QComboBox::showPopup();
}

//------------------------------------------------------------------------------
void LCQDataComboBox::hidePopup(void)
{
  mFlagPopupOn = false;
  mDataReader->readRequest();
  QComboBox::hidePopup();
}
