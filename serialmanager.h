#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QTextStream>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    SerialManager(){}
bool setupSerial(const QString &serialName = {});
private slots:
    void readNew();
private:

    void lineToSerial(QString data){
        serial.write(data.append("\r\n").toLatin1());
        serial.waitForBytesWritten();
    }
    QSerialPort serial;
    QTextStream serialIO;

    static const QMap <QString, std::function<QString()>> gets;
};

#endif // SERIALMANAGER_H
