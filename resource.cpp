#include "resource.h"
#include <QJsonArray>
#include <QDateTime>

Resource::Resource(const QString &resName) : name(resName)
{
}

bool Resource::addRes(QStringList path, Resource *res)
{
    if(path.isEmpty()) return false;
    auto curLevel = path.first();
    path.removeFirst();
    std::shared_ptr<Resource> &target = subResources[curLevel];
    if(!target.get())
        target.reset(new Resource());
    if(!path.isEmpty())
            return target->addRes(path, res);
    else{
        target.reset(res);
        return true;
    }
}


std::weak_ptr<Resource> Resource::getRes(QStringList path)
{
    std::weak_ptr<Resource> weak;
    if(path.isEmpty()){
        return  {};
    }
    auto curLevel = path.first();
    path.removeFirst();
    if(subResources.contains(curLevel))
        weak = subResources[curLevel];
    return weak;
}

QJsonObject Resource::toJsonObject(bool allTree) const
{
    QJsonObject thisObject;
    thisObject[name] = value();
    QJsonArray subRes;
    for(auto it = subResources.constBegin(); it != subResources.constEnd(); ++it){
        QJsonObject subObj;
        auto resPtr = it.value();
        subObj[it.key()] = (resPtr.get()&&allTree) ? resPtr->toJsonObject(allTree) : QJsonObject();
        subRes.append(subObj);
    }
    thisObject["resources"] = subRes;
    return thisObject;
}



Date::Date() : Resource("time")
{

}

QString Date::value() const
{
    return QDateTime::currentDateTime().toString();
}

void Date::setValue(const QString &name, const QString &value)
{
    if(name == this->name){
        system(QString("echo %1").arg(value).toUtf8().constData());
    }
}

QString Eth::value(const QString &name) const
{

}
