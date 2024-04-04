#include "server.h"
#include "requesti.h"

#include <QTcpSocket>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

#include <QHttpMultiPart>
#include <QHttpPart>

Server::Server(QObject *parent)
{
    connect(this, &Server::newConnection, this, &Server::onNewConnection);
    listen(QHostAddress::Any, 8001);

}

void Server::onNewConnection()
{
    QTcpSocket *newClient = nextPendingConnection();
    newClient->waitForReadyRead();
    RequestI req(newClient->readAll());
    qDebug() << req.toString() << req.URI;
    {QTextStream os(newClient);
    os.setAutoDetectUnicode(true);
    os << "HTTP/1.1 200 Ok\r\n" <<
        "Content-Type: application/json; charset=UTF-8\r\n" <<
        "\r\n" <<
          "{\"field\" : \"test\"}";
    }
    newClient->close();
}

void Server::sendAck()
{
}
