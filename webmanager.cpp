#include "webmanager.h"
#include <QJsonDocument>
#include <memory>

QString WebManager::processRequest(const RequestI &request)
{
    QSharedPointer<Resource> targetResource;
    QString ret = reply(NotFound, RequestI::UNKNOWN);
    if(request.URI.isEmpty()) targetResource = rootResource;
    else targetResource = rootResource->getRes(request.URI).toStrongRef();
    if(!targetResource.isNull()){
        switch (request.method) {
        case RequestI::GET:{
            ret = reply(Ok, RequestI::GET);
            ret+=QString::fromLatin1(QJsonDocument(targetResource->toJsonObject()).toJson(QJsonDocument::JsonFormat::Indented));
            break;
        }
        case RequestI::PATCH:{
            if(targetResource->setValue(request.body))
                ret = reply(Ok, RequestI::PATCH);
            break;
        }
        default:
            ret = reply(NotFound, request.method);
            break;
    }
    }
    return ret;
}
