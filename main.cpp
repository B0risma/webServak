#include <QCoreApplication>
#include "server.h"
#include "serialmanager.h"
#include <QObject>

constexpr auto helpMsg = "No serial device specified! \n\r -D <tty device> to specify serial device";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server httpServer(0);
    SerialManager serialServer;
    {
        QString serialDevice;
        for(int i = 0; i < argc; ++i){
            if(QString(argv[i]) == "-D")
                serialDevice = argv[i+1];
        }
        if(!serialServer.setupSerial(serialDevice)){
            qDebug() << helpMsg;
            return -1;
        }
    }
    QObject::connect(&serialServer, &SerialManager::onClose, [&a](){
        qDebug() << "close server";
        a.quit();});
    return a.exec();

}
