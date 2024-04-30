#include "serialmanager.h"
#include "resource.h"

#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>
#include <string>

const QMap <QString, std::function<QString()>> SerialManager::gets{
    {QLatin1String("date"), &Date::getTime},
};


SerialManager::SerialManager()
{
    setupSerial();
}

void SerialManager::readNew()
{
    QString newData = serialIO.readAll();
    qDebug()  << "new serial data" << newData;
    serialIO << QString("echo: ").append(newData);
    if(newData.isEmpty()) return;

    if(newData == "?"){
        QString out;
        QTextStream strBuf(&out, QIODevice::WriteOnly);
        for(auto it = gets.constBegin(); it != gets.constEnd(); ++it){
            strBuf << it.key() << ":" << it.value()() << '\n';
        }
        serialIO << out;
        qDebug() << out;
        return;
    }

    if(newData.at(0) == '?'){
        const QString &resource = newData.section("? ",1, 1);
        serialIO << gets[resource]();
        qDebug() << gets[resource]();
    }

}
void SerialManager::setupSerial()
{
    const auto &serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        qDebug() << "\n"
                 << "Port:" << portInfo.portName() << "\n"
                 << "Location:" << portInfo.systemLocation();
    }
    if(serialPortInfos.isEmpty()){
        qDebug() << "no available COMs";
        return;
    }
    serial.setPort(serialPortInfos.first());
    serial.setBaudRate(115200);
    if(!serial.open(QIODevice::ReadWrite))
        qDebug() << "error open tty";
    serialIO.setDevice(&serial);
    connect(&serial, &QSerialPort::readyRead, this, &SerialManager::readNew);
}
