#include "webmanager.h"
#include <QJsonDocument>

QString WebManager::processRequest(const RequestI &request)
{
    std::weak_ptr<Resource> targetResource;
    QString ret = reply(NotFound, RequestI::UNKNOWN);
    if(request.URI.isEmpty()) targetResource = rootResource;
    else targetResource = rootResource->getRes(request.URI);
    if(!targetResource.expired()){
        switch (request.method) {
        case RequestI::GET:{
            ret = reply(Ok, RequestI::GET);
            ret+=QString::fromLatin1(QJsonDocument(targetResource.lock()->toJsonObject()).toJson(QJsonDocument::JsonFormat::Indented));
            break;
        }
        case RequestI::POST:{
            break;
        }
        default:
            ret = reply(NotFound, request.method);
            break;
    }
    }
    return ret;
}
