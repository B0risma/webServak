#include "serialmanager.h"
#include "resource.h"
#include "interfaces.h"

#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>
#include <string>

const QMap <QString, std::function<QString()>> SerialManager::gets{
    {QLatin1String("date"), &Date::getTime},
    {QLatin1String("sysinfo"), &SysInfo::stringData},
    {QLatin1String("interfaces"), &IFManager::stringData},
};




void SerialManager::readNew()
{
    QString newData = serialIO.readAll();

    newData.remove('\n');
    newData.remove('\r');
    qDebug()  << "new serial data" << newData;
    lineToSerial(newData);
    if(newData.isEmpty()) return;

    if(newData == "?"){
        for(auto it = gets.constBegin(); it != gets.constEnd(); ++it){
            QString out;
            QTextStream strBuf(&out, QIODevice::WriteOnly);
            strBuf << it.key() << ":" << it.value()() << '\n';
            lineToSerial(out);
            qDebug() << out;
        }
        return;
    }

    if(newData.at(0) == '?'){
        const QString &resource = newData.section("? ",1, 1);
        if(gets.contains(resource)){
            lineToSerial(gets[resource]());
            qDebug() << gets[resource]();
        }
    }

}
bool SerialManager::setupSerial(const QString &serialName)
{
//    const auto &serialPortInfos = QSerialPortInfo::availablePorts();
//    for (const QSerialPortInfo &portInfo : serialPortInfos) {
//        qDebug() << "\n"
//                 << "Port:" << portInfo.portName() << "\n"
//                 << "Location:" << portInfo.systemLocation();
//    }
//    if(serialPortInfos.isEmpty()){
//        qDebug() << "no available COMs";
//        return;
//    }
//    serial.setPort(/*serialPortInfos.first()*/ );
    serial.setPortName(serialName);
    serial.setBaudRate(115200);
    if(!serial.open(QIODevice::ReadWrite)){
        qDebug() << "error open tty";
        return false;
    }
    serialIO.setDevice(&serial);
    qDebug() << "server listen" << serialName;
    lineToSerial("Server started\r");
    connect(&serial, &QSerialPort::readyRead, this, &SerialManager::readNew);
    return true;
}
