#pragma once

#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent);

    void onNewConnection();
public slots:
    void sendAck();
};

