#include "serialmanager.h"
#include "resource.h"
#include "interfaces.h"

#include <QSerialPortInfo>
#include <QTextStream>
#include <QChar>
#include <QDebug>
#include <string>

SerialBuf::SerialBuf(QObject *parent, const QString &device) : QSerialPort(parent)
{
    constexpr uint32_t baudRate = 115200;
    setPortName(device);
    setBaudRate(baudRate);
    if(!open(QIODevice::ReadWrite)){
        qDebug() << "error open tty";
    }
    serialIO.setDevice(this);
    qDebug() << "tty opened" << device << "baud:" << baudRate << "8N1";
    connect(this, &QSerialPort::readyRead, this, &SerialBuf::procInput);
}

void SerialBuf::writeLine(const QString &line)
{
    write(QString(line + "\r\n").toLatin1());
    waitForBytesWritten();
}

void SerialBuf::procInput()
{
    QString newData = serialIO.readAll();
    newData.remove('\n');
    newData.remove('\r');
    const bool empty = newData.isEmpty();
    if(empty)
        newData.append("\r\n");
    write(newData.toLatin1());
    waitForBytesWritten();
    if(empty){
        emit newLine(inputBuffer);
        inputBuffer.clear();
    }
    else if(newData == QChar(0x007F))
        inputBuffer.remove(inputBuffer.size()-1, 1);
    else
        inputBuffer.append(newData);

}

const QMap <QString, std::function<QString()>> SerialManager::gets{
    {QLatin1String("date"), &Date::getTime},
    {QLatin1String("sysinfo"), &SysInfo::stringData},
    {QLatin1String("interfaces"), &IFManager::stringData},
};


const QMap <QString, std::function<bool(QString)>> SerialManager::sets{
    {QLatin1String("date"), &Date::setTime},
    {QLatin1String("interfaces"), &IFManager::setStringData},
    };

void SerialManager::readNew(const QString &newData)
{
    if(newData.isEmpty()) return;
    if(newData == "?"){
        for(auto it = gets.constBegin(); it != gets.constEnd(); ++it){
            QString out;
            QTextStream strBuf(&out, QIODevice::WriteOnly);
            strBuf << it.key() << ":" << it.value()() << '\n';
            serial->writeLine(out);
        }
        return;
    }

    if(newData.at(0) == '?'){
        const QString &resource = newData.section("? ",1, 1);
        if(gets.contains(resource)){
            serial->writeLine(gets[resource]());
        }
        else  serial->writeLine(QString("unknown ") + resource);;
    }
    else if (newData.section(' ', 0,0) == "set"){
        const auto& res = newData.section(' ', 1,1);
        if(sets.contains(res)){
            qDebug() << "try set" << newData.section(' ', 2,2);
            if(sets[res](newData.section(' ', 2)))
                serial->writeLine(gets[res]());
            else serial->writeLine("invalid data");
        }
        else serial->writeLine(QString("unknown ") + res);
    }
    else if(newData == "close")
        emit onClose();
    else serial->writeLine("unknown command");
}

SerialManager::~SerialManager()
{
    serial->writeLine("close server");
}

bool SerialManager::setupSerial(const QString &serialName)
{
//    constexpr uint32_t baudRate = 115200;
//    serial.setPortName(serialName);
//    serial.setBaudRate(baudRate);
//    if(!serial.open(QIODevice::ReadWrite)){
//        qDebug() << "error open tty";
//        return false;
//    }
//    serialIO.setDevice(&serial);
//    qDebug() << "server listen" << serialName << "baud:" << baudRate << "8N1";
    serial = new SerialBuf(this, serialName);
    if(!serial->isOpen())
        return false;
    serial->writeLine("Server started");
    connect(serial, &SerialBuf::newLine, this, &SerialManager::readNew);
//    connect(&serial, &QSerialPort::readyRead, this, &SerialManager::readNew);
    return true;
}


