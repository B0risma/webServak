#include "server.h"
#include "requesti.h"
#include "webmanager.h"


#include <QTcpSocket>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>


Server::Server(QObject *parent)
{
    connect(this, &Server::newConnection, this, &Server::onNewConnection);
    listen(QHostAddress::Any, 8001);
    fillResources();

}

void Server::onNewConnection()
{
    QTcpSocket *newClient = nextPendingConnection();
    newClient->waitForReadyRead();
    RequestI req(newClient->readAll());
    qDebug() << req.toString() << req.URI;
    {QTextStream os(newClient);
    os.setAutoDetectUnicode(true);
    std::shared_ptr<Resource> res;
    if(!getResource(req.URI).expired())
        res = getResource(req.URI).lock();
    else res.reset(new Resource);

    WebManager man(resources);
    qDebug() << man.processRequest(req);
    os << man.processRequest(req);
    }
    newClient->close();
}

void Server::sendAck()
{

}

void Server::fillResources()
{
    resources.reset(new Resource);
    {
        resources->addRes({"level1"}, new Resource);
        resources->addRes({"date"}, new Date);
    }

}

std::weak_ptr<Resource> Server::getResource(QStringList path)
{
 if(path.isEmpty())
     return resources;
 else
    return resources->getRes(path);
}
