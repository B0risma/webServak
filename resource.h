#pragma once
#include <QMap>
#include <memory>
#include <QJsonObject>

enum ResourceType : quint8{
    Ethernet,
    Serial,
    Text
};

//!Базовый класс данных с возможной вложенностью
//! По сути JSONObject - может потом и заменю
class Resource
{
public:
    Resource(){}
    virtual ~Resource() = default;
    ResourceType type = ResourceType::Text;
    //!Удаление объекта, если существовал
    bool deleteRes(const QStringList &path);
    //!Вставка только в этот уровень
    void addRes(const QString &curPath, const QString &data);
    //!Рекурсивная вставка, если такой путь существует
//    bool addRes(const QStringList &path, const QString &data);
    bool addRes(QStringList path, Resource *res);
    //!Указатель на ресурс этого уровня
//    const Resource &getRes(const QString &curPath) const;
    QWeakPointer<Resource> getRes(QStringList path);
    QMap<QString, QSharedPointer<Resource>> subResources;

    virtual QJsonObject data(const QJsonObject& requestData) const;
    virtual bool setData(const QJsonObject &data){}
};

class Date : public Resource{
public:
    Date();
    virtual ~Date() = default;
    QJsonObject data(const QJsonObject& requestData) const override;
    bool setData(const QJsonObject &data) override;
    static QString getTime();
    static bool setTime(const QString& newTime);
};

class SysInfo : public Resource{
public:
    SysInfo(){};
    virtual ~SysInfo() = default;
    QJsonObject data(const QJsonObject& requestData) const override;
    bool setData(const QJsonObject &data) override{return false;}
    static QString stringData();
    static QStringList CPUTemp();
};
