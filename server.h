#pragma once
#include "resource.h"
#include "webmanager.h"

#include <QTcpServer>
#include <QSharedPointer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent);

    void onNewConnection();
public slots:
    void sendAck();


private:
    Resource * fillResources();
//    QWeakPointer<Resource> getResource(QStringList path);
    WebManager manager;
};

