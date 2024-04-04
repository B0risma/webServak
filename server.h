#pragma once
#include "resource.h"

#include <QTcpServer>
#include <memory>

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
    std::unique_ptr<Resource> resources;
};

