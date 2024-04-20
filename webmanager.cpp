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
            qDebug() << request.body;
            targetResource->setValue("time", "2024-04-20T10:10:10");
            break;
        }
        default:
            ret = reply(NotFound, request.method);
            break;
    }
    }
    return ret;
}
