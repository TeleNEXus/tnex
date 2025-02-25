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
#ifndef LQVALIDATORU32_H
#define LQVALIDATORU32_H

#include <QValidator>

class LQValidatorU32 : public QValidator
{
  Q_OBJECT;

  quint32 mMin;
  quint32 mMax;

public:
  explicit LQValidatorU32(QObject *parent = nullptr);
  virtual ~LQValidatorU32();
  virtual QValidator::State validate(QString &input, int &pos) const override;
  void setRange(quint32 _min, quint32 _max);

};

#endif // LQVALIDATORU32_H
