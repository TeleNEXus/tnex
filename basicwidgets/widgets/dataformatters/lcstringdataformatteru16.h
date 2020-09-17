﻿#ifndef LCSTRINGDATAFORMATTERUINT16_H
#define LCSTRINGDATAFORMATTERUINT16_H

#include "lcstringdataformatterintbase.h"
#include <QIntValidator>

class LCStringDataFormatterU16 : public LCStringDataFormatterIntBase
{
private:
    QIntValidator mValidator;   //Контроллер диапазона ввода.

public:
    explicit LCStringDataFormatterU16(  int     _fieldWidth     = 0,
                                  QChar   _fillChar       = QChar(' '),
                                  int     _base           = 10,
                                  QChar   _fillCharUndef  = QChar(),
                                  QChar   _fillCharWrong  = QChar());

    virtual QString     toString(const QByteArray& _data) override;
    virtual QString     normalizeString(const QString& _str) override;
    virtual QByteArray  toBytes(const QString& _str) override;
    virtual QString     undefStateString() override;
    virtual QValidator* validator() override;
};

#endif // LCSTRINGDATAFORMATTERUINT16_H
