#ifndef INTERFACES_H
#define INTERFACES_H
#include "resource.h"

class IFManager : public Resource{
public:
    IFManager(){}
    virtual ~IFManager() = default;
    QJsonObject data(const QJsonObject& requestData) const override;
    bool setData(const QJsonObject &data) override;
    static QString stringData();
    //! ip/mask
    static QString getIPv4(const QString &ifName);
    int setIPv4(const QString &name, const QString &addrmask);
    static QStringList getIPv6(const QString &ifName);
    int setIPv6(const QString &name, const QString &addr);
    static QStringList interfaces();
};

#endif // INTERFACES_H
