#ifndef LOGGER_H
#define LOGGER_H
#include <QObject>
#include <QTextStream>
#include <QFile>
#include <QDateTime>

class Logger : public QObject
{
public:
    Logger(QObject *parent, const QString &logName = {});

    template <typename T>
    Logger & operator<<(T in){
        if(!logFile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
            qDebug() << "Can`t open/create log file";
        }
        else{
            fileStr.setDevice(&logFile);
            fileStr << QDateTime::currentDateTime().toString() << ' '<<  in << '\n';
        }
        logFile.close();
        return *this;
    }
//    template <typename T>
//    QTextStream &	operator>>(T out){
//        return fileStr >> out;
//    }
private:
    QTextStream fileStr;
    QFile logFile;
};

#endif // LOGGER_H
