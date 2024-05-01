#include "logger.h"
#include <QDir>
#include <QFile>
#include <QDebug>


Logger::Logger(QObject *parent, const QString &logName) : QObject(parent),
    logFile(QDir::currentPath().append("/%1Log").arg(logName))
{
    qDebug() << "Log file" << logFile.fileName();
}
