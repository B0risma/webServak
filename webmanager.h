#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include "resource.h"
#include "requesti.h"
//! Тот самый менеджер, который умеет работать с веб траффиком
class WebManager
{
public:
    enum ReplyStat : quint8{
        NotFound = 0,
        NoAuth = 1,
        Ok=2,
        Error,
        NoImplement
    };
    WebManager() {}
    constexpr const char* reply(const ReplyStat &stat, const RequestI::Method method) const{
        switch(stat){
        case Ok:
            if(method == RequestI::POST)
                return "HTTP/1.1 201 Created\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";
            return "HTTP/1.1 200 Ok\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";
            break;
        case NoAuth:
            return "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"server\"\r\n\r\n";
            break;
        case NotFound:
            return "HTTP/1.1 404 Not Found\r\n\r\n";
            break;
        case Error:
            return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            break;
        case NoImplement:
            return "HTTP/1.1 501 Not Implementedr\n\r\n";
            break;
        }
    }
    QString processRequest(const RequestI &request);
    QSharedPointer<Resource> rootResource;
};

QString decodeBase64(const QString base64);

#endif // WEBMANAGER_H
