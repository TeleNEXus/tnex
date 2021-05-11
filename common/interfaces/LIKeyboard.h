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
#ifndef LIKEYBOARD_H_ 
#define LIKEYBOARD_H_

#include "LIWindow.h"
#include "LIRemoteDataSource.h"
#include "LIDataFormatter.h"
#include <QSharedPointer>

class LIKeyboard : public LIWindow
{
public:
    LIKeyboard(){}
    virtual ~LIKeyboard(){}
    /* virtual QSharedPointer<LIRemoteDataSource> getStreamSource()const = 0; */
    /* virtual QString getStreamName()const = 0; */
    /* virtual QSharedPointer<LIDataFormatter> getDataFormatter() const = 0; */
    virtual void connectWidget(QWidget* _widget) = 0;
    virtual void disconnectWidget(QWidget* _widget) = 0;
    virtual void setData(const QString& _data) = 0;
};

#endif // LIKEYBOARD_H_

