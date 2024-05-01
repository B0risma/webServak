#include "server.h"
#include "requesti.h"
#include "webmanager.h"
#include "interfaces.h"



#include <QTcpSocket>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSerialPortInfo>


Server::Server(QObject *parent)
{
    connect(this, &Server::newConnection, this, &Server::onNewConnection);
    listen(QHostAddress::Any, 8001);
    manager.rootResource.reset(fillResources());

    httpLogger = new Logger(this, "http");
    *httpLogger << "ServerStarted on port" << 8001;
}

void Server::onNewConnection()
{
    QTcpSocket *newClient = nextPendingConnection();
    newClient->waitForReadyRead();
    const auto received = newClient->readAll();
    RequestI req(received);
    QTextStream os(newClient);
    os.setAutoDetectUnicode(true);
    const QString& reply = manager.processRequest(req);
    os << reply;
    newClient->close();
    *httpLogger << "request" << received;
    *httpLogger << "reply" << reply;
}


Resource *Server::fillResources()
{
    auto resources = new Resource;
    {
//        resources->addRes({"level1"}, new Resource);
        resources->addRes({"date"}, new Date);
        resources->addRes({"interfaces"}, new IFManager);
        resources->addRes({"sysinfo"}, new SysInfo);
    }
    return resources;
}

