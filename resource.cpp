#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <QScopedPointer>

#include <sys/time.h>




#include <string>
#include <memory>

Resource::Resource(const QString &resName) : name(resName)
{
}

bool Resource::addRes(QStringList path, Resource *res)
{
    if(path.isEmpty()) return false;
    auto curLevel = path.first();
    path.removeFirst();
    QSharedPointer<Resource> &target = subResources[curLevel];
    if(!target.get())
        target.reset(new Resource());
    if(!path.isEmpty())
            return target->addRes(path, res);
    else{
        target.reset(res);
        return true;
    }
}


QWeakPointer<Resource> Resource::getRes(QStringList path)
{
    QWeakPointer<Resource> weak;
    if(path.isEmpty()){
        return  {};
    }
    auto curLevel = path.first();
    path.removeFirst();
    if(subResources.contains(curLevel))
        weak = subResources[curLevel];
    return weak;
}

//QJsonObject Resource::toJsonObject(const QString &obj) const
//{
//    bool allTree = name.isEmpty();
//    QJsonObject thisObject;
//    if(obj.isEmpty())
//        thisObject[name];
//    QJsonArray subRes;
//    for(auto it = subResources.constBegin(); it != subResources.constEnd(); ++it){
//        QJsonObject subObj;
//        auto resPtr = it.value();
//        subObj[it.key()] = (resPtr.get()&&allTree) ? resPtr->toJsonObject(allTree) : QJsonObject();
//        subRes.append(subObj);
//    }
//    thisObject["resources"] = subRes;
//    return thisObject;
//}

//bool Resource::value(QStringList path, QJsonValue &value) const
//{
//    if(!path.isEmpty()){
//        const QString &res = path.pop_front();
//        auto resPtr = subResources.value(res);
//        if(resPtr.isNull()) return false;
//        return resPtr->value(path, value);
//    }
//    QJsonObject forValue;
//    QJsonArray subRes;
//    for(auto it = subResources.constBegin();
//         it != subResources.constEnd(); ++t)
//        subRes.append(it.key());
//    forValue["resources"] = subRes;
//    value = forValue;
//    return true;
//}

QJsonObject Resource::data(const QJsonObject &requestData) const
{
    QJsonObject resData;
    if(requestData.isEmpty()){
        QJsonArray subRes;
        for(auto it = subResources.constBegin();
             it != subResources.constEnd(); ++it)
            subRes.append(it.key());
        resData["resources"] = subRes;
    }
    return resData;
}





Date::Date() : Resource("time")
{
}

QJsonObject Date::data(const QJsonObject &requestData) const
{
    auto obj = Resource::data(requestData);
    QDateTime curTime = QDateTime::fromSecsSinceEpoch(time(nullptr));
    obj["date"] = curTime.toString(Qt::DateFormat::ISODate);
    return obj;
}

//bool Date::value(QStringList path, QJsonValue &value)
//{
//    QDateTime curTime = QDateTime::fromSecsSinceEpoch(time(nullptr));
//    value = curTime.toString(Qt::DateFormat::ISODate);
//    return true;
//}

bool Date::setValue(const QJsonObject &data)
{
    qDebug() << name << "setValue" << data;
    if(data.contains("time")){
        setTime(data["time"].toString());
        qDebug() << "OK";
    }
    else return false;
    return true;
}

void Date::setTime(const QString &timeStr)
{
    auto time = QDateTime::fromString(timeStr, Qt::DateFormat::ISODate);
    const time_t curTime = time.toSecsSinceEpoch();
    timeval newTime{curTime};
    settimeofday(&newTime, nullptr);
}
