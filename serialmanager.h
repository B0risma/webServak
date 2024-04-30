#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QTextStream>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    SerialManager();

private slots:
    void readNew();
private:
    void setupSerial();
    QSerialPort serial;
    QTextStream serialIO;

    static const QMap <QString, std::function<QString()>> gets;
};

#endif // SERIALMANAGER_H
