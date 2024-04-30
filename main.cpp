#include <QCoreApplication>
#include <server.h>
#include "serialmanager.h"

constexpr auto helpMsg = "No serial device specified! \n\r -D <tty device> to specify serial device";

int main(int argc, char *argv[])
{
    QString serialDevice;
    for(int i = 0; i < argc; ++i){
//        qDebug() << "option" << i << argv[i];
        if(QString(argv[i]) == "-D")
            serialDevice = argv[i+1];
    }
    QCoreApplication a(argc, argv);
    Server httpServer(0);
    SerialManager serialServer;
//    qDebug() << "serial" << serialDevice;
    if(!serialServer.setupSerial(serialDevice)){
        qDebug() << helpMsg;
        return -1;
    }
    return a.exec();

}
