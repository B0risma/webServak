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
private:
    void onNewConnection();
    Resource * fillResources();
    WebManager manager;
    Logger *httpLogger;
};

