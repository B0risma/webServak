#pragma once
#include <QMap>
#include <memory>

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
    Resource(Resource *parent = 0);
    ResourceType type = ResourceType::Text;
    //!Удаление объекта, если существовал
    bool deleteRes(const QStringList &path);
    //!Вставка только в этот уровень
    void addRes(const QString &curPath, const QString &data);
    //!Рекурсивная вставка, если такой путь существует
    bool addRes(const QStringList &path, const QString &data);
    //!Указатель на ресурс этого уровня
    std::unique_ptr<Resource> getRes(const QString &curPath);
    Resource* parent = 0;
    QMap<QString, std::unique_ptr<Resource>> subResources;
    QString data;

};

