#pragma once
#include "resource.h"
#include "webmanager.h"
#include "serialmanager.h"
#include "logger.h"

#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent);
public slots:
    void sendAck();
private:
    void onNewConnection();
    Resource * fillResources();
    WebManager manager;
    Logger *httpLogger;
};

