#pragma once
#include "resource.h"

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
    void fillResources();
    QWeakPointer<Resource> getResource(QStringList path);
    QSharedPointer<Resource> resources;
};

