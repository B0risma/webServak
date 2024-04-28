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

IFManager::IFManager() : Resource("interfaces")
{
}

QString IFManager::value(const QString &name) const
{

}

std::string IFManager::getIPv4(const std::string &ifName) const
{
    constexpr auto sockFamily = AF_INET;
    auto sock = socket(sockFamily, SOCK_DGRAM, 0);
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, ifName.c_str());
    if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        qDebug() << "fail request IP";
    std::unique_ptr<char[]> ifIP(new char[INET_ADDRSTRLEN]);
    auto *addr = (sockaddr_in*)&ifr.ifr_addr;
    return {inet_ntop(sockFamily, &addr->sin_addr, ifIP.get(), INET_ADDRSTRLEN )};
}

int IFManager::setIPv4(const std::string &name, const std::string &addrStr)
{
    constexpr auto sockFamily = AF_INET;
    auto sock = socket(sockFamily, SOCK_DGRAM, 0);
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, name.c_str());
    auto *addr = (sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = sockFamily;
    if(addrStr.size() > INET_ADDRSTRLEN){
        qDebug() << "Invalid ip format";
        return -1;
    }
    inet_pton(sockFamily, addrStr.c_str(), &addr->sin_addr);
    return ioctl(sock, SIOCSIFADDR, &ifr);
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

QStringList IFManager::getIPv6(const std::string &ifName) const{
    std::string command{"ip -6 addr show dev "};
    command.append(ifName);
    FILE *fd = popen(command.c_str(), "r");
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

int del6Addr(const std::string &ifName, const std::string &addrStr){
    std::string command{"ip -6 addr del "};
    command.append(addrStr);
    command += " dev ";
    command.append(ifName);
    FILE *fd = popen(command.c_str(), "r");
    QTextStream out(fd, QIODevice::ReadOnly);
    qDebug() << "file" << out.readAll();
    if(out.readAll().isEmpty()) return -1;
    pclose(fd);
    return 0;
}

int IFManager::setIPv6(const std::string &ifName, const std::string &addrStr)
{
//    qDebug() << "addresses" << get6Addrs(ifName);
    const auto addrs = getIPv6(ifName);
    for(const auto &addr : addrs){
        del6Addr(ifName, addr.toStdString());
    }
//    qDebug() << "addresses after" << get6Addrs(ifName);

    std::string command{"ip -6 addr add "};
    command.append(addrStr);
    command += " dev ";
    command.append(ifName);
    FILE *fd = popen(command.c_str(), "r");
    QTextStream out(fd, QIODevice::ReadOnly);
    if(out.readAll().isEmpty()) return -1;
    pclose(fd);
    return 0;
}
