#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include <sys/time.h>

#include <string>
#include <memory>

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


Date::Date()
{
}

QJsonObject Date::data(const QJsonObject &requestData) const
{
    auto obj = Resource::data(requestData);
    obj["date"] = getTime();
    return obj;
}

bool Date::setData(const QJsonObject &data)
{
    qDebug() << "Date::setData" << data;
    if(data.contains("date")){
        return setTime(data["date"].toString());
    }
    return false;
}

QString Date::getTime()
{
    return QDateTime::fromSecsSinceEpoch(time(nullptr)).toString(Qt::DateFormat::ISODate);
}

bool Date::setTime(const QString &timeStr)
{
    const auto &time = QDateTime::fromString(timeStr, Qt::DateFormat::ISODate);
    const time_t curTime = time.toSecsSinceEpoch();
    timeval newTime{curTime};
    if(settimeofday(&newTime, nullptr) != 0)
        return false;
    return true;
}

QJsonObject SysInfo::data(const QJsonObject &requestData) const
{
    QJsonObject ret;
    ret["processor temp"] = procTemp();
    return ret;
}

float SysInfo::procTemp() const
{
    QFile tempInfo("/sys/class/thermal/thermal_zone0/temp");
    if(!tempInfo.open(QIODevice::ReadOnly))
        return 0;
    QTextStream fileStr(&tempInfo);
    return fileStr.readLine().toFloat()/1000;
}
