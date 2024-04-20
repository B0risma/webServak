#ifndef WEBMANAGER_H
#define WEBMANAGER_H

#include "resource.h"
#include "requesti.h"
//! Тот самый менеджер, который умеет работать с веб траффиком
class WebManager
{
public:
    enum ReplyStat : quint8{
        Ok,
        NotFound
    };
    WebManager(/*QWeakPointer<Resource> targetRes*/) {
//        auto newRes = new Resource;
//        *newRes= *(targetRes.toStrongRef());
//        rootResource.reset(newRes);
    }
    constexpr const char* reply(const ReplyStat &stat, const RequestI::Method method) const{
        switch(stat){
        case Ok:
            if(method == RequestI::POST)
                return "HTTP/1.1 201 Created\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";
            return "HTTP/1.1 200 Ok\r\nContent-Type: application/json; charset=UTF-8\r\n\r\n";
            break;
        case NotFound:
            return "HTTP/1.1 404 Not Found\r\n\r\n";
        }
    }
    QString processRequest(const RequestI &request);


    QSharedPointer<Resource> rootResource;
};

#endif // WEBMANAGER_H
