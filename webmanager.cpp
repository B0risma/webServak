#include "webmanager.h"
#include <QJsonDocument>
#include <memory>

QString WebManager::processRequest(const RequestI &request)
{
    QSharedPointer<Resource> targetResource;
    QString ret = reply(NotFound, request.method);
    if(request.URI.isEmpty()) targetResource = rootResource;
    else targetResource = rootResource->getRes(request.URI).toStrongRef();
    if(!targetResource.isNull()){
        switch (request.method) {
        case RequestI::GET:{
            ret+=QString::fromLatin1(QJsonDocument(targetResource->data({})).toJson(QJsonDocument::JsonFormat::Indented));
            break;
        }
        case RequestI::PATCH:{
            if(targetResource->setData(request.body))
                ret = reply(Ok, RequestI::PATCH);
            else qDebug() << "error";
            break;
        }
        default:
            ret = reply(NotFound, request.method);
            break;
    }
    }
    return ret;
}
