#include "server.h"
#include "requesti.h"
#include "webmanager.h"
#include "interfaces.h"



#include <QTcpSocket>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>


Server::Server(QObject *parent)
{
    connect(this, &Server::newConnection, this, &Server::onNewConnection);
    listen(QHostAddress::Any, 8001);
    manager.rootResource.reset(fillResources());

}

void Server::onNewConnection()
{
    QTcpSocket *newClient = nextPendingConnection();
    newClient->waitForReadyRead();
    RequestI req(newClient->readAll());
    qDebug() << req.toString() << req.URI;
    QTextStream os(newClient);
    os.setAutoDetectUnicode(true);
    os << manager.processRequest(req);
    newClient->close();
}

void Server::sendAck()
{

}

Resource *Server::fillResources()
{
    auto resources = new Resource;
    {
        resources->addRes({"level1"}, new Resource);
        resources->addRes({"date"}, new Date);
        resources->addRes({"interfaces"}, new IFManager);
        resources->addRes({"sysinfo"}, new SysInfo);
    }
    return resources;
}

//QWeakPointer<Resource> Server::getResource(QStringList path)
//{
// if(path.isEmpty())
//     return resources;
// else
//    return resources->getRes(path);
//}
