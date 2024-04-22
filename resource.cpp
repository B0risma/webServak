#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <ifaddrs.h>
#include <iomanip>

#include <sys/time.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <net/if.h>

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

QJsonObject Resource::toJsonObject(bool allTree) const
{
    QJsonObject thisObject;
    thisObject[name] = value();
    QJsonArray subRes;
    for(auto it = subResources.constBegin(); it != subResources.constEnd(); ++it){
        QJsonObject subObj;
        auto resPtr = it.value();
        subObj[it.key()] = (resPtr.get()&&allTree) ? resPtr->toJsonObject(allTree) : QJsonObject();
        subRes.append(subObj);
    }
    thisObject["resources"] = subRes;
    return thisObject;
}





Date::Date() : Resource("time")
{

}


QString Date::value(const QString &name) const
{
    QDateTime curTime = QDateTime::fromSecsSinceEpoch(time(nullptr));
    return curTime.toString(Qt::DateFormat::ISODate);
}

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


Eth::Eth() : Resource("Ethernet")
{

}

QString Eth::value(const QString &name) const
{

}
