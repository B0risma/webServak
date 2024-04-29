#include "interfaces.h"

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <cstdint>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QJsonArray>


QJsonObject IFManager::data(const QJsonObject &requestData) const
{
    auto obj = Resource::data({});
    const auto &ifNames = interfaces();
    for(const auto &intName : ifNames){
        QJsonObject interface;
        interface["inet"] = getIPv4(intName);
        interface["inte6"] = QJsonArray::fromStringList(getIPv6(intName));
        obj[intName] = interface;
    }
    return obj;
}

bool IFManager::setData(const QJsonObject &data)
{
    if(data.isEmpty()) return false;
    bool ret = false;
    const auto &ifNames = interfaces();
    for(const auto &ifName : ifNames)
    {
        if(!data.contains(ifName)) continue;
        ret = true;
        const auto &ifData = data.value(ifName).toObject();
        if(ifData.contains("inet")){
            if(setIPv4(ifName, ifData.value("inet").toString()) == 0)
                ret &= true;
            else ret = false;
        }
        if(ifData.contains("inet6")){
            if(setIPv6(ifName, ifData.value("inet6").toString()) == 0)
                ret &= true;
            else ret = false;
        }
    }
    return ret;
}



QString IFManager::getIPv4(const QString &ifName) const
{
    constexpr auto sockFamily = AF_INET;
    auto sock = socket(sockFamily, SOCK_DGRAM, 0);
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, ifName.toStdString().c_str());
    if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        qDebug() << "fail request IP";
    std::unique_ptr<char[]> ifIP(new char[INET_ADDRSTRLEN]);
    auto *addr = (sockaddr_in*)&ifr.ifr_addr;
    QString ret {inet_ntop(sockFamily, &addr->sin_addr, ifIP.get(), INET_ADDRSTRLEN )};
    if(ioctl(sock, SIOCGIFNETMASK, &ifr) < 0)
        qDebug() << "fail request mask";
    addr = (sockaddr_in*)&ifr.ifr_netmask;
    ret+='/';
    memset(ifIP.get(), 0, INET_ADDRSTRLEN);
    ret.append(inet_ntop(sockFamily, &addr->sin_addr, ifIP.get(), INET_ADDRSTRLEN));
    return ret;
}

int IFManager::setIPv4(const QString &name, const QString &addrmask)
{
    const QString &addrStr = addrmask.section('/', 0, 0);
    const QString &maskStr = addrmask.section('/', 1, 1);
    constexpr auto sockFamily = AF_INET;
    auto sock = socket(sockFamily, SOCK_DGRAM, 0);
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, name.toStdString().c_str());
    auto *addr = (sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = sockFamily;
    if(addrStr.size() > INET_ADDRSTRLEN){
        qDebug() << "Invalid ip format";
        return -1;
    }
    inet_pton(sockFamily, addrStr.toStdString().c_str(), &addr->sin_addr);
    auto ret = ioctl(sock, SIOCSIFADDR, &ifr);
    if(ret) qDebug() << "fail set ipv4";

    addr = (sockaddr_in *)&ifr.ifr_netmask;
    addr->sin_family = sockFamily;
    if(addrStr.size() > INET_ADDRSTRLEN){
        qDebug() << "Invalid mask format";
        return -1;
    }
    inet_pton(sockFamily, maskStr.toStdString().c_str(), &addr->sin_addr);
    ret = ioctl(sock, SIOCSIFNETMASK, &ifr);
    if(ret) qDebug() << "fail set mask";
    return ret;
}

//std::string IFManager::getIPv6(const std::string &ifName) const
//{
//    std::string command{"ip -6 addr show dev "};
//    command.append(ifName);
//    FILE *fd = popen(command.c_str(), "r");
//    QTextStream out(fd, QIODevice::ReadOnly);
//    QStringList splitted = out.readAll().split(' ', Qt::SkipEmptyParts);
//    pclose(fd);
//    QString addr;
//    for(auto it = splitted.constBegin(); it != splitted.constEnd(); ++it){
//        if(*it == "inet6" && it != splitted.constEnd()){
//            addr = *(++it);
//            break;
//        }
//    }
//    return addr.toStdString();
//}

QStringList IFManager::getIPv6(const QString &ifName) const{
    QString command{"ip -6 addr show dev "};
    command.append(ifName);
    FILE *fd = popen(command.toStdString().c_str(), "r");
    QTextStream out(fd, QIODevice::ReadOnly);
    QStringList splitted = out.readAll().split(' ', Qt::SkipEmptyParts);
    pclose(fd);
    QStringList addrs;
    for(auto it = splitted.constBegin(); it != splitted.constEnd(); ++it){
        if(*it == "inet6" && it != splitted.constEnd()){
            addrs.push_back(*(++it));
        }
    }
    return addrs;
}

int del6Addr(const QString &ifName, const QString &addrStr){
    QString command{"ip -6 addr del "};
    command.append(addrStr);
    command += " dev ";
    command.append(ifName);
    FILE *fd = popen(command.toStdString().c_str(), "r");
    QTextStream out(fd, QIODevice::ReadOnly);
    qDebug() << "file" << out.readAll();
    if(!out.readAll().isEmpty()) return -1;
    pclose(fd);
    return 0;
}

int IFManager::setIPv6(const QString &ifName, const QString &addrStr)
{
//    qDebug() << "addresses" << get6Addrs(ifName);
    const auto addrs = getIPv6(ifName);
    for(const auto &addr : addrs){
        del6Addr(ifName, addr);
    }
//    qDebug() << "addresses after" << get6Addrs(ifName);
    QString command{"ip -6 addr add "};
    command.append(addrStr);
    command += " dev ";
    command.append(ifName);
    FILE *fd = popen(command.toStdString().c_str(), "r");
    QTextStream out(fd, QIODevice::ReadOnly);
    if(!out.readAll().isEmpty()) return -1;
    pclose(fd);
    return 0;
}

QStringList IFManager::interfaces() const
{
    QStringList ifNames;
    ifconf conf;
    constexpr int testLen = 10*sizeof(ifreq);
    conf.ifc_len = testLen;
    conf.ifc_ifcu.ifcu_req = new ifreq[10];
    auto sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(ioctl(sock, SIOCGIFCONF, &conf) < 0){
        perror("SIOCIFCONF");
        return ifNames;
    }
    else if(conf.ifc_len != testLen){
        delete[] conf.ifc_ifcu.ifcu_req;
        conf.ifc_ifcu.ifcu_req = new ifreq[conf.ifc_len/sizeof(ifreq)];
        if(ioctl(sock, SIOCGIFCONF, &conf) != 0)
            return ifNames;
    }
    const ifreq *ifr = conf.ifc_ifcu.ifcu_req;
    size_t i = 0;
    while(ifr && i < (conf.ifc_len/sizeof(ifreq))){
        ifNames << ifr->ifr_ifrn.ifrn_name;
        ++ifr;
        ++i;
    }
    delete[] conf.ifc_ifcu.ifcu_req;
    return ifNames;
}
