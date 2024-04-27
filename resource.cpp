#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <QScopedPointer>

#include <sys/time.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>


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




IFManager::IFManager() : Resource("interface")
{
}

QString IFManager::value(const QString &name) const
{

}

std::string IFManager::getIPv4(const std::string &ifName) const
{
    auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct ifreq ifr = {};
    ifName.data();
    strcpy(ifr.ifr_name, ifName.c_str());
    if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        qDebug() << "fail request IP";
    std::unique_ptr<char[]> ifIP(new char[INET_ADDRSTRLEN]);
    auto *addr = (sockaddr_in*)&ifr.ifr_addr;
    return {inet_ntop(AF_INET, &addr->sin_addr, ifIP.get(), INET_ADDRSTRLEN )};
}

int IFManager::setIPv4(const std::string &name, const std::string &addrStr)
{
    auto sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, name.c_str());
    auto *addr = (sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    if(addrStr.size() > INET_ADDRSTRLEN){
        qDebug() << "Invalid ip format";
        return -1;
    }
    inet_pton(AF_INET, addrStr.c_str(), &addr->sin_addr);
    return ioctl(sock, SIOCSIFADDR, &ifr);
}
