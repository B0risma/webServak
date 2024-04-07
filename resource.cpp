#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <ifaddrs.h>
#include <iomanip>
#include <ctime>


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
    const std::time_t now = time(nullptr);
    return {asctime(std::localtime(&now))};
}

void Date::setValue(const QString &name, const QString &value)
{
    if(true){
        timespec *tp;
        clock_gettime(CLOCK_REALTIME, tp);
        tp->tv_sec+=65;
        clock_settime(CLOCK_REALTIME, tp);
    }
}

QString Eth::value(const QString &name) const
{

}
