#pragma once
#include <QStringList>
//! Базовый интерфейс для обработки клиентских запросов
//!
class RequestI
{
public:
//    RequestI();
    RequestI(const QByteArray &reqData);
    QString toString() const;
    enum Method : quint8{
        GET,
        POST,
        UNKNOWN
    } method = UNKNOWN;
    bool isValid = false;
    QStringList URI;
    QStringList header;
    QStringList body; //Времянка - по сути JSON

};

