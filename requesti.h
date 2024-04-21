#pragma once
#include <QStringList>
#include <QMetaEnum>
#include <QJsonObject>
//! Базовый интерфейс для обработки клиентских запросов
//!
class RequestI
{
Q_GADGET
public:
    RequestI(const QByteArray &reqData);
    QString toString() const;
    enum Method : qint8{
        UNKNOWN = -1,
        HEAD, //Заголовки без данных (не нужно)
        GET, //Получение данных
        POST, //Изменение данных
        PATCH, //Использовать вместо POST
        OPTIONS //Принимаемые запросы
    } method = UNKNOWN;
    Q_ENUM(Method)
    bool isValid = false;
    QStringList URI;
    QStringList header;
    QJsonObject body;
};
