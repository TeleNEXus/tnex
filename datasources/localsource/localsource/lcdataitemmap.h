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
#ifndef LCDATAITEMMAP_H_
#define LCDATAITEMMAP_H_

#include <QMap>
#include <QBitArray>
#include <QSharedPointer>
#include <QList>

class LCQLocalDataReader;
class LCQLocalDataWriter;

class LCDataItemMap
{
private:

  using TReadersMap =QMap<QWeakPointer<LCQLocalDataReader>, QString>;

  class CDataItemBase
  {
  protected:
    QList<QWeakPointer<LCQLocalDataReader>> mReadersList;
    void notifyReaders(const QByteArray& _data);

  public:
    CDataItemBase(){}

    void connectReader(QSharedPointer<LCQLocalDataReader> _sw_reader);
    void disconnectReader(QSharedPointer<LCQLocalDataReader> _sw_reader);

    virtual QByteArray getData() = 0;
    virtual int setData(const QByteArray& _data) = 0;
  }; 

  class CDataItemBytes : public CDataItemBase
  {
  private:
    QByteArray mData;
  public:
    CDataItemBytes() = delete;
    CDataItemBytes(const QByteArray& _data):
      mData(_data){}
    virtual QByteArray getData() override { return mData; }
    virtual int setData(const QByteArray& _data) override;
  };

  class CDataItemBits : public CDataItemBase
  {
  private:
    QBitArray mData;
  public:
    CDataItemBits() = delete;
    CDataItemBits(const QBitArray& _data) :
      mData(_data){}
    virtual QByteArray getData() override;
    virtual int setData(const QByteArray& _data) override;
  };

  QMap<QString, QSharedPointer<CDataItemBase>> mDataMap;
  
public:

  LCDataItemMap();

  void addItem(const QString& _id, const QByteArray& _data);
  void addItem(const QString& _id, const QBitArray& _data);

  void readData(QSharedPointer<LCQLocalDataReader> _sp_reader);
  void writeData(
      QSharedPointer<LCQLocalDataWriter> _sp_writer, 
      const QByteArray& _data);

  void connectReader(QSharedPointer<LCQLocalDataReader> _sp_reader);
  void disconnectReader(QSharedPointer<LCQLocalDataReader> _sp_reader);
};

#endif //LCDATAITEMMAP_H_
