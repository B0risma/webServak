#include <QCoreApplication>
#include <server.h>
#include "serialmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    Server serv(0);
    SerialManager ser;
    return a.exec();

}
