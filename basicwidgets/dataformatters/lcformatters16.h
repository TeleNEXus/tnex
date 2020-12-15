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
#ifndef LCFORMATTERINT16_H
#define LCFORMATTERINT16_H

#include "lcformatterintbase.h"
#include <QIntValidator>

class LCFormatterS16 : public LCFormatterIntBase
{
private:

    QIntValidator mValidator;   //Контроллер диапазона ввода.

public:
    explicit LCFormatterS16(    
        int     _fieldWidth     = 0,
        QChar   _fillChar       = QChar(' '),
        int     _base           = 10);


    virtual QString     toString(const QByteArray& _data) override;
    QString normalize(const QString& _str) override;
    virtual QByteArray  toBytes(const QString& _str) override;
    virtual QValidator* validator() override;
};


#endif // LCFORMATTERINT16_H
