#ifndef INTERFACES_H
#define INTERFACES_H
#include "resource.h"

class IFManager : public Resource{
public:
    IFManager();
    virtual ~IFManager() = default;
    QJsonObject data(const QJsonObject& requestData) const override;
    //! ip/mask
    QString getIPv4(const QString &ifName) const;
    int setIPv4(const QString &name, const QString &addr, const QString &mask);
    QStringList getIPv6(const QString &ifName) const;
    int setIPv6(const QString &name, const QString &addr);
};

#endif // INTERFACES_H
