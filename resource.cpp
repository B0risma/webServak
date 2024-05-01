#include "resource.h"
#include <QJsonArray>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include <sys/time.h>

#include <memory>

bool Resource::addRes(QStringList path, Resource *res)
{
    if(path.isEmpty()) return false;
    auto curLevel = path.first();
    path.removeFirst();
    QSharedPointer<Resource> &target = subResources[curLevel];
    if(!target.get())
        target.reset(new Resource());
    if(!path.isEmpty())
            return target->addRes(path, res);
    else{
        target.reset(res);
        return true;
    }
}


QWeakPointer<Resource> Resource::getRes(QStringList path)
{
    QWeakPointer<Resource> weak;
    if(path.isEmpty()){
        return  {};
    }
    auto curLevel = path.first();
    path.removeFirst();
    if(subResources.contains(curLevel))
        weak = subResources[curLevel];
    return weak;
}

//QJsonObject Resource::toJsonObject(const QString &obj) const
//{
//    bool allTree = name.isEmpty();
//    QJsonObject thisObject;
//    if(obj.isEmpty())
//        thisObject[name];
//    QJsonArray subRes;
//    for(auto it = subResources.constBegin(); it != subResources.constEnd(); ++it){
//        QJsonObject subObj;
//        auto resPtr = it.value();
//        subObj[it.key()] = (resPtr.get()&&allTree) ? resPtr->toJsonObject(allTree) : QJsonObject();
//        subRes.append(subObj);
//    }
//    thisObject["resources"] = subRes;
//    return thisObject;
//}

//bool Resource::value(QStringList path, QJsonValue &value) const
//{
//    if(!path.isEmpty()){
//        const QString &res = path.pop_front();
//        auto resPtr = subResources.value(res);
//        if(resPtr.isNull()) return false;
//        return resPtr->value(path, value);
//    }
//    QJsonObject forValue;
//    QJsonArray subRes;
//    for(auto it = subResources.constBegin();
//         it != subResources.constEnd(); ++t)
//        subRes.append(it.key());
//    forValue["resources"] = subRes;
//    value = forValue;
//    return true;
//}

QJsonObject Resource::data(const QJsonObject &requestData) const
{
    QJsonObject resData;
    if(requestData.isEmpty()){
        QJsonArray subRes;
        for(auto it = subResources.constBegin();
             it != subResources.constEnd(); ++it)
            subRes.append(it.key());
        resData["resources"] = subRes;
    }
    return resData;
}


Date::Date()
{
}

QJsonObject Date::data(const QJsonObject &requestData) const
{
    auto obj = Resource::data(requestData);
    obj["date"] = getTime();
    return obj;
}

bool Date::setData(const QJsonObject &data)
{
    if(data.contains("date")){
        return setTime(data["date"].toString());
    }
    return false;
}

QString Date::getTime()
{
    return QDateTime::fromSecsSinceEpoch(time(nullptr)).toString(Qt::DateFormat::ISODate);
}

bool Date::setTime(const QString &timeStr)
{
    const auto &time = QDateTime::fromString(timeStr, Qt::DateFormat::ISODate);
    const time_t curTime = time.toSecsSinceEpoch();
    timeval newTime{curTime};
    if(settimeofday(&newTime, nullptr) != 0)
        return false;
    return true;
}

QJsonObject SysInfo::data(const QJsonObject &requestData) const
{
    QJsonObject ret;
    const auto &coreTemps = CPUTemp();
    {
        QJsonObject CPUtemp;
        for(int i = 0; i < coreTemps.size(); ++i){
            CPUtemp[QString("CPU%1").arg(i)] = coreTemps.at(i);
        }
        ret["CPU temperature"] = CPUtemp;
    }
    return ret;
}

QString SysInfo::stringData()
{
    QString out;
    QTextStream str(&out, QIODevice::WriteOnly);
    const auto &coreTemps = CPUTemp();
    for(int i = 0; i < coreTemps.size(); ++i){
        str << QString("CPU%1: ").arg(i) << coreTemps.at(i) << '\n';
    }
    return out;
}

QStringList SysInfo::CPUTemp()
{
    QStringList cpuTemps;
    QDir cpuHwmon;
    {
        QDir hwmon("/sys/class/hwmon");
        if(hwmon.exists()){
            hwmon.setFilter(QDir::Dirs);
            for(const auto &hwmonNum : hwmon.entryList({"hwmon*"})){
                QDir objHwmon(hwmon.absolutePath()+ '/' + hwmonNum);
                const auto &tempFiles = objHwmon.entryList({"temp*_input*"});
                if(tempFiles.isEmpty() && !objHwmon.exists("name"))
                    continue;
                QFile name(objHwmon.absolutePath() + '/' + "name");
                if(!name.open(QIODevice::ReadOnly))
                    continue;
                QTextStream fStream(&name);
                if(fStream.readLine() != "coretemp")
                    continue;
                name.close();
                cpuHwmon.setPath(objHwmon.absolutePath());
                break;
            }
        }
    }


    for(const auto& tFileName : cpuHwmon.entryList({"temp*_input"})){
        QFile tempInfo(cpuHwmon.absoluteFilePath(tFileName));
        if(!tempInfo.open(QIODevice::ReadOnly))
            continue;
        QTextStream fileStr(&tempInfo);
        cpuTemps << QString::number(fileStr.readLine().toFloat()/1000);
    }
    return cpuTemps;
}
