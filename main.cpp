#include <QCoreApplication>
#include <server.h>
#include <string>
#include "interfaces.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    new Server(0);
    return a.exec();

}
