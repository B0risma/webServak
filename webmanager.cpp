#include "webmanager.h"
#include <QJsonDocument>
#include <QBitArray>
#include <string>

const char* username = "user";
const char* password = "password";

QString WebManager::processRequest(const RequestI &request)
{
    QSharedPointer<Resource> targetResource;
    QString ret = reply(NotFound, request.method);
    if(request.URI.isEmpty()) targetResource = rootResource;
    else targetResource = rootResource->getRes(request.URI).toStrongRef();
    if(!targetResource.isNull()){
        QString key{};
        for(const auto &par : request.header){
            if(par.contains("Authorization") && par.contains("Basic")){
                key = decodeBase64(par.section(' ', 2,2));
                break;
            }
        }
        if(key.section(':',0,0) == username
            || key.section(':',1,1) == password){
            switch (request.method) {
            case RequestI::GET:{
                ret = reply(Ok, RequestI::GET);
                ret+=QString::fromLatin1(QJsonDocument(targetResource->data({})).toJson(QJsonDocument::JsonFormat::Indented));
                break;
            }
            case RequestI::PATCH:{
                if(targetResource->setData(request.body))
                    ret = reply(Ok, RequestI::PATCH);
                else  ret = reply(Error, RequestI::PATCH);
                break;
            }
            default:
                ret = reply(NoImplement, request.method);
                break;
            }
        }
        else ret = reply(NoAuth, request.method);
    }
    return ret;
}


const QString base64Map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


QString decodeBase64(const QString base64)
{
    QBitArray rawBuf(base64.size()*6);
    for(int i = 0; i < base64.size(); ++i){
    const auto &ch = base64[i];
    qint8 byte = base64Map.indexOf(ch);
//    qDebug() << "char" << ch << "num" << byte;
    if(byte < 0) continue;
    QBitArray rawByte = QBitArray::fromBits((char*)(&byte), 8);
    for(int ii = 0; ii <6; ++ii){
            rawBuf.setBit(i*6+ii, rawByte.testBit(5-ii));
    }
    }
    std::string ret;
    char tmpCh = 0;
    for(int i = 0; i < rawBuf.size(); ++i){
    quint8 mod = i%8;
    tmpCh |= ((rawBuf.testBit(i)*1) << (7-mod));
    if(mod == 7){
            if(tmpCh != 0)
                ret.push_back(tmpCh);
            tmpCh = 0;
    }
    }
    return QString::fromStdString(ret);
}
