#include "lcstringdataformatterhex.h"
#include <QDebug>
#include <functional>

LCStringDataFormatterHex::LCStringDataFormatterHex( 
                                        int     _size,
                                        QChar   _separator,
                                        QChar   _fillCharUndef,
                                        QChar   _fillCharWrong) :  
                                            mSize(_size),
                                            mSeparator(_separator),
                                            mFillCharUndef(_fillCharUndef),
                                            mFillCharWrong(_fillCharWrong)
{
}

//------------------------------------------------------------------------------toString
QString LCStringDataFormatterHex::toString(const QByteArray& _data)
{
    QString str;
    if(_data.size() < 1)
    {
        return QString(msFillCharWrongDefLength, mFillCharWrong);
    }

    for(int i = (_data.size() - 1); i >= 0; i--)
    {
        if(mSeparator.isNull())
        {
            str = str + QString("%1").arg(
                            ((quint8*)_data.constData())[i], 2, 16, QChar('0'));
        }
        else
        {
            str = str + QString("%1%2").arg(
                            ((quint8*)_data.constData())[i], 2, 16, QChar('0')).
                                arg(mSeparator);
        }
    }

    if(!mSeparator.isNull())
    {
        str.resize(str.length() -1);
    }
    return str;
}

//------------------------------------------------------------------------------normalizeString
QString LCStringDataFormatterHex::normalizeString(const QString& _instr)
{
    QString out_string = _instr;

    //Удаляем разделительные символы.
    out_string.remove(QRegExp(  QString("[ ]{1,}|[_]{1,}|[%1]{1,}")
                                    .arg(mSeparator) ));

    //Проверяем на строку нулевой длины.
    if( out_string.size() <= 0)
    {
        return QString();
    }

    //Проверяем на наличие нецифровых значений.
    if( out_string.contains(QRegExp("[^0-9,a-f]{1,}")))
    {
        //Если присутствуют посторонние символы, 
        //возвращаем пустую строку.
        return QString();
    }

    //Переводим строку в нижний регистр.
    out_string  = out_string.toLower(); 
 
    if(mSize <= 0)
    {
        //Если размер не задан, то производим нормализацию до четного количества
        //символов и проверку значений.
        if( (out_string.size() % 2 ) != 0)
        {
            //Добавляем незначащий ноль.
            out_string.insert(0, '0');
        }
        return out_string;
    }
    //Если установлен размер данных.
    //Удаляем незначащие нули.
    out_string.remove(QRegExp("^[0]{1,}"));
    //Добавляем незначащий ноль для сохранения четности количества цифр.
    if( (out_string.size() % 2 ) != 0)
    {
        //Добавляем незначащий ноль.
        out_string.insert(0, '0');
    }
    int str_byte_size = out_string.length() / 2;

    if( str_byte_size > mSize )
    {
        out_string.remove(0, (str_byte_size - mSize) * 2);
    }
    else if( mSize > str_byte_size )
    {
        out_string.insert(0, QString((mSize - str_byte_size) * 2, '0'));
    }
    return out_string;
}

//------------------------------------------------------------------------------toBytes
QByteArray LCStringDataFormatterHex::toBytes(const QString& _str)
{
    QByteArray out_array;
    QString instr = _str;

    //Удаление всех незначащих символов.
    instr.remove(   QRegExp(
                        QString(
                            "[ ]{1,}|[_]{1,}|[%1]{1,}").arg(mSeparator) ));

    // Проверка на нулевую строку.
    if(instr.length() == 0) return out_array;

    //Переводим в нижний регистр
    instr  = instr.toLower(); 

    // Проверка на нечисловые значения.
    if(instr.contains(QRegExp("[^0-9,a-f]{1,}")))
    {
        return out_array;
    }
    //Проверяем на количество цифр.
    if( (instr.length() % 2) != 0)
    {
        //Добавляем незначащий ноль.
        instr.insert(0, '0');
    }

    if(mSize > 0)
    {
        int str_byte_size = instr.length() / 2;
        if(str_byte_size > mSize )
        {
            instr.remove(0, (str_byte_size - mSize) * 2);
        }
        else if(mSize > str_byte_size)
        {
            instr.insert(0, QString((mSize - str_byte_size) * 2, '0'));
        }
    }

    for(int i = (instr.length() - 2); i >= 0; i -=2 )
    {
        char data = instr.mid(i,2).toShort(nullptr, 16);
        out_array.append(1,data);
    }
    return out_array;
}

//------------------------------------------------------------------------------undefStateString
QString     LCStringDataFormatterHex::undefStateString()
{
    return QString(msFillCharUndefDefLength, mFillCharUndef);
}

