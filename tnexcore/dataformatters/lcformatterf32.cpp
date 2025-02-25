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
#include "lcformatterf32.h"
#include "math.h"


//==============================================================================LCQDataStringFormatterFloat32
LCFormatterF32::LCFormatterF32(
    int     _fieldWidth,
    char    _format,
    int     _precision,
    int     _decimals,
    QChar   _fillChar,
    float   _lessIsZero) :   mFieldWidth(_fieldWidth),
  mFormat(_format),
  mPrecision(_precision),
  mFillChar(_fillChar),
  mLessIsZero(fabs(_lessIsZero))
{
  mValidator.setRange(
      (-1.0f)*std::numeric_limits<float>::max(), 
      std::numeric_limits<float>::max(), 
      _decimals);
  mValidator.setLocale(QLocale::c());
}

//------------------------------------------------------------------------------
LCFormatterF32::~LCFormatterF32()
{
}

//------------------------------------------------------------------------------toString
QString LCFormatterF32::toString(const QByteArray& _data)
{
  if(_data.size() < 4)
  {
    QString str = "Wrong";
    wrongState(str);
    return str;
  }

  float data = *((float*)(_data.constData()));

  if(mLessIsZero != 0.0f)
  {
    if(fabs(data) < mLessIsZero) data = 0.0f;
  }

  return QString("%1").arg(data, mFieldWidth, mFormat, mPrecision, mFillChar);
}


//------------------------------------------------------------------------------toBytes
QByteArray LCFormatterF32::toBytes(const QString& _str)
{
  bool ok = false;
  float r = ((float)_str.toFloat(&ok));
  if(!ok) return QByteArray();
  return QByteArray((char*)(&r), 4);
}

//------------------------------------------------------------------------------validator
QValidator* LCFormatterF32::validator()
{
  return &mValidator;
}

//------------------------------------------------------------------------------setLessIsZero
void LCFormatterF32::setLessIsZero(float _lessIsZero)

{
  mLessIsZero = fabs(_lessIsZero);
}

