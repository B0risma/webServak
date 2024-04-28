#ifndef INTERFACES_H
#define INTERFACES_H
#include "resource.h"

class IFManager : public Resource{
public:
    IFManager();
    virtual ~IFManager() = default;
    QString value(const QString &name = {}) const override;
    std::string getIPv4(const std::string &ifName) const;
    int setIPv4(const std::string &name, const std::string &addr);
    QStringList getIPv6(const std::string &ifName) const;
    int setIPv6(const std::string &name, const std::string &addr);
};

#endif // INTERFACES_H
