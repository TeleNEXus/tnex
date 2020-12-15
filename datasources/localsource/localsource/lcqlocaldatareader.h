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
#ifndef LCQLOCALDATAREADER_H_
#define LCQLOCALDATAREADER_H_

#include "lqextendevent.h"
#include "lcqlocalsourcehiden.h"
#include "LIRemoteDataSource.h"
#include "LIRemoteDataReader.h"

class LCQLocalDataReader final: public QObject, public LIRemoteDataReader
{
  Q_OBJECT;
private:
  using LTReadAction = LIRemoteDataSource::LTReadAction;

public:
  //----------------------------------------------------------------------------CQEventDataRead
  class CQEventDataIsRead : public QEvent
  {
    __LQ_EXTENDED_QEVENT_DECLARATION

  public:
      QSharedPointer<QByteArray> mspData;
      LERemoteDataStatus mStatus;

      explicit CQEventDataIsRead(
          const QByteArray& _data,
          LERemoteDataStatus _status);

      explicit CQEventDataIsRead(LERemoteDataStatus _status);
  };

private:

  QString mDataName;
  LTReadAction mReadAction;
  QWeakPointer<LCQLocalSourceHiden> mwpDataSource;
  QWeakPointer<LCQLocalDataReader> mwpThis;

  LCQLocalDataReader() = delete;

  LCQLocalDataReader(
      const QString& _dataName, 
      LTReadAction _readAction,
      QSharedPointer<LCQLocalSourceHiden> _dataSource);

public:
  virtual ~LCQLocalDataReader();

  static QSharedPointer<LCQLocalDataReader> 
    create(
        const QString& _dataName, 
        LTReadAction _readAction,
        QSharedPointer<LCQLocalSourceHiden> _dataSource);

  virtual void readRequest() override;
  virtual void connectToSource() override;
  virtual void disconnectFromSource() override;

public:

  void notifyListener(const QByteArray& _data);
  void notifyListener(LERemoteDataStatus _status);
  QString getDataName(){ return mDataName; }

private:
  virtual void customEvent(QEvent* _event) override;

};

#endif // LCQLOCALDATAREADER_H_
