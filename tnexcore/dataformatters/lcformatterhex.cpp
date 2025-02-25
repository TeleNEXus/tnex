/* 
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
#include "lcformatterhex.h"
#include <QDebug>
#include <functional>
//==============================================================================
LCFormatterHex::
CValidator::CValidator(int _size, QChar _separator, QObject *_parent) : 
  QValidator(_parent),
  mSize(_size),
  mSeparator(_separator)
{
}

//-----------------------------------------------------------------------------
QValidator::State 
LCFormatterHex::
CValidator::validate(QString &_input, int& _pos) const
{
  Q_UNUSED(_pos);
  QString str = _input;
  str.remove(QRegExp(QString("[ _%1]").arg(mSeparator)));
  if(str.isNull())
  {
    return State::Intermediate;
  }

  if( str.contains( QRegExp( QString("[^0-9a-fA-F]{1,}"))) )
  {
    return State::Invalid;
  }

  if( ( mSize > 0) && (str.size() > (2 * mSize) ))
  {
    return State::Invalid;
  }

  return State::Acceptable;
}

//==============================================================================
LCFormatterHex::LCFormatterHex( 
    int     _size,
    QChar   _separator) :  
  mValidator(_size, _separator)
{
}

//------------------------------------------------------------------------------
LCFormatterHex::
LCFormatterHex( const LCFormatterHex& _formatter)
{
  mValidator.mSize          = _formatter.mValidator.mSize;         
  mValidator.mSeparator     = _formatter.mValidator.mSeparator;    
}
//------------------------------------------------------------------------------
LCFormatterHex::~LCFormatterHex()
{
}

  LCFormatterHex& 
LCFormatterHex::operator=(const LCFormatterHex& _formatter)
{
  mValidator.mSize          = _formatter.mValidator.mSize;         
  mValidator.mSeparator     = _formatter.mValidator.mSeparator;    
  return *this;
}

//------------------------------------------------------------------------------toString
QString LCFormatterHex::toString(const QByteArray& _data)
{
  QString str;
  if(_data.size() < 1)
  {
    QString str = "Wrong";
    wrongState(str);
    return str;
  }

  for(int i = (_data.size() - 1); i >= 0; i--)
  {
    if(mValidator.mSeparator.isNull())
    {
      str = str + QString("%1").arg(
          ((quint8*)_data.constData())[i], 2, 16, QChar('0'));
    }
    else
    {
      str = str + QString("%1%2").arg(
          ((quint8*)_data.constData())[i], 2, 16, QChar('0')).
        arg(mValidator.mSeparator);
    }
  }

  if(!mValidator.mSeparator.isNull())
  {
    str.resize(str.length() -1);
  }
  return str;
}

//------------------------------------------------------------------------------toBytes
QByteArray LCFormatterHex::toBytes(const QString& _str)
{
  QByteArray out_array;
  QString instr = _str;

  //Удаление всех незначащих символов.
  instr.remove(   QRegExp(
        QString(
          "[ ]{1,}|[_]{1,}|[%1]{1,}").arg(mValidator.mSeparator) ));

  // Проверка на нулевую строку.
  if(instr.length() == 0) return out_array;

  //Переводим в нижний регистр
  instr  = instr.toLower(); 

  // Проверка на нечисловые значения.
  if(instr.contains(QRegExp("[^0-9a-f]{1,}")))
  {
    return out_array;
  }
  //Проверяем на количество цифр.
  if( (instr.length() % 2) != 0)
  {
    //Добавляем незначащий ноль.
    instr.insert(0, '0');
  }
  if(mValidator.mSize > 0)
  {
    int str_byte_size = instr.length() / 2;
    if(str_byte_size > mValidator.mSize )
    {
      instr.remove(0, (str_byte_size - mValidator.mSize) * 2);
    }
    else if(mValidator.mSize > str_byte_size)
    {
      instr.insert(0, QString((mValidator.mSize - str_byte_size) * 2, '0'));
    }
  }

  for(int i = (instr.length() - 2); i >= 0; i -=2 )
  {
    char data = instr.mid(i,2).toShort(nullptr, 16);
    out_array.append(1,data);
  }
  return out_array;
}

//------------------------------------------------------------------------------setSize
void LCFormatterHex::setSize(int _size)
{
  mValidator.mSize = _size;
}

//------------------------------------------------------------------------------setSeparator
void LCFormatterHex::setSeparator(QChar _separator)
{
  mValidator.mSeparator = _separator;
}
