#include "requesti.h"
#include <QTextStream>
#include <QMetaEnum>

#include <QJsonDocument>

RequestI::RequestI(const QByteArray &data)
{
    QTextStream s(data);
    QString firstLine = s.readLine();
    header.push_back(firstLine);
    int httpPos = firstLine.indexOf("HTTP");
    if(!httpPos){
        isValid = false;
        return;
    }
    isValid = true;
    firstLine.remove(httpPos, firstLine.size() - httpPos);
    auto reqType = firstLine.split('/').value(0).remove(' ');

    QMetaEnum enumer = QMetaEnum::fromType<RequestI::Method>();
    method = (Method)enumer.keyToValue(reqType.toUtf8().constData());

    firstLine.remove(reqType);
    firstLine.remove(' ');
    URI = firstLine.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);


    for(QString line = s.readLine(); !line.isEmpty() && !s.atEnd(); line = s.readLine())
        header.push_back(line);
//    qDebug() << "header " << header;

    if(!s.atEnd())
    {
        QString reqBody;
        while(!s.atEnd()){
            reqBody.append(s.readLine());
        }
        QJsonParseError error;
        auto jsonReq = QJsonDocument::fromJson(reqBody.toLocal8Bit(), &error);
//        qDebug() << "json body: " << jsonReq.toJson();
        if(error.error != QJsonParseError::NoError)
            qDebug() << "error:" << error.errorString();
        body = jsonReq.object();
    }

}

QString RequestI::toString() const
{
    QString str;
    QTextStream s(&str);
    QMetaEnum methodObj = QMetaEnum::fromType<RequestI::Method>();
    s << "METHOD: " << methodObj.valueToKey(method) << '\n';
    s << "URL ";
    for(const auto &line:URI)
        s<<line << '/';
    s << '\n';
    s << "all header: \n";
    for(const auto &line:header)
        s<<line << '\n';
    s << "body: \n";
    {
        s << QJsonDocument(body).toJson();
    };
    return str;
}
