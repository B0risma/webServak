#include "requesti.h"
#include <QTextStream>
#include <QMetaEnum>

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

}

QString RequestI::toString() const
{
    QString str;
    QTextStream s(&str);
    s << "METHOD(GET(0), POST(1))" << method << '\n';
    s << "URL ";
    for(const auto &line:URI)
        s<<line << '/';
    s << '\n';
    s << "header: \n";
    for(const auto &line:header)
        s<<line << '\n';
    s << "body: \n";
    for(const auto &line:body)
        s<<line << '\n';
    return str;
}
