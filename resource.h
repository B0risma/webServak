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
    Resource(const QString &resName);
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
    virtual QJsonObject toJsonObject(bool allTree = false) const;

    QString name = "Int";
    virtual QString value(const QString &name = {}) const {return "test";}
    virtual void setValue(const QString &name, const QString &value){}
    virtual bool setValue(const QJsonObject &data){}
};

class Date : public Resource{
public:
    Date();
    virtual ~Date() = default;
    QString value(const QString &name = {}) const override;
    bool setValue(const QJsonObject &data) override;
    void setTime(const QString& newTime);
};


class Eth : public Resource{
   public:
    Eth();
    virtual ~Eth() = default;
    QString value(const QString &name = {}) const override;
};
