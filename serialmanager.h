#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QTextStream>

class SerialBuf : public QSerialPort{
  Q_OBJECT
public:
  SerialBuf(QObject *parent, const QString& device);
    void writeLine(const QString &line);
signals:
  void newLine(const QString line);
private slots:
  void procInput();
private:
  QTextStream serialIO;
  QString inputBuffer;
};

class SerialManager : public QObject
{
    Q_OBJECT
public:
    SerialManager(){}
    ~SerialManager();
    bool setupSerial(const QString &serialName = {});
signals:
    void onClose();
private slots:
    void readNew(const QString &command);
private:

//    void lineToSerial(QString data){
//        serial.write(data.append("\r\n").toLatin1());
//        serial.waitForBytesWritten();
//    }
//    QSerialPort serial;
    SerialBuf *serial;


    static const QMap <QString, std::function<QString()>> gets;
    static const QMap <QString, std::function<bool(QString)>> sets;
};

#endif // SERIALMANAGER_H
