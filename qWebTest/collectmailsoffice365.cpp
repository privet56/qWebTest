#include "collectmailsoffice365.h"
#include "urldownloaderwp.h"
#include "helpers.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

CollectMailsOffice365::CollectMailsOffice365(QObject *parent, QNetworkAccessManager* pMan) : QObject(parent)
{
    this->m_pMan = pMan;
}
QString CollectMailsOffice365::getUrl(QString sFolderId, QString sSubFolder, QString sUrl)
{
    sUrl = helpers::str_isempty(sUrl, true) ? helpers::GetCFG("url", qApp, 0) : sUrl;
    if(!helpers::str_isempty(sFolderId, true) &&
       !helpers::str_isempty(sSubFolder,true))
    {
        sUrl = helpers::makeAbsFN(sUrl, sFolderId+"/"+sSubFolder);
    }

    return urldownloaderwp::load(this, sUrl, helpers::GetCFG("un" , qApp, 0), helpers::GetCFG("pwd", qApp, 0));
}
QString CollectMailsOffice365::getFolderId(QString sFolderId, QString sFolderName2Return)
{
    QString sUrlContent = this->getUrl(sFolderId, helpers::str_isempty(sFolderId, true) ? "" : "ChildFolders", "");
    return getId(sUrlContent, sFolderName2Return);
}
QString CollectMailsOffice365::getId(QString sJson, QString sFolderName)
{
    QJsonDocument djson = QJsonDocument::fromJson(sJson.toUtf8());
    QJsonObject ojson = djson.object();
    QJsonArray ajson = ojson["value"].toArray();
    foreach (const QJsonValue& value, ajson)
    {
        QJsonObject obj = value.toObject();

        if((obj["DisplayName"].toString() == sFolderName) &&
            !helpers::str_isempty(obj["Id"].toString(), true))
        {
            return obj["Id"].toString();
        }
    }
    helpers::log("off365:getId foldernf:'"+sFolderName+"' --------------\n"+sJson+"\n --------------", LOG_WRN, qApp, 0);
    return "";
}

void CollectMailsOffice365::collectMailInfo(QString sFolderId, QString sNextPageFullUrl, int iPage)
{
    QString sUrlContent = this->getUrl(sFolderId, helpers::str_isempty(sNextPageFullUrl, true) ? "messages" : "", sNextPageFullUrl);
    if(helpers::str_isempty(sUrlContent, true))
    {
        helpers::log("off365:collect r3 !messages in '"+sFolderId+"'", LOG_WRN, qApp, 0);
        return;
    }
    QJsonDocument djson = QJsonDocument::fromJson(sUrlContent.toUtf8());
    QJsonObject ojson = djson.object();
    QJsonArray ajson = ojson["value"].toArray();
    foreach (const QJsonValue& value, ajson)
    {
        QJsonObject obj = value.toObject();
        handleMail(obj);
    }

    QString sNextPage = ojson["@odata.nextLink"].toString();
    if(!helpers::str_isempty(sNextPage, true))
    {
        if(helpers::GetCFG("break", qApp, 0) == "1")
        {
            helpers::log("off365:collect BREAK ==> DO NOT call next page("+QString::number(iPage)+"): '"+sNextPage+"'", LOG_WRN, qApp, 0);
            return;
        }

        helpers::log("collectMailInfo:collect call next page("+QString::number(iPage)+"): '"+sNextPage+"'", LOG_INF, qApp, 0);
        collectMailInfo("", sNextPage, iPage+1);
        return;
    }
}
void CollectMailsOffice365::handleMail(QJsonObject mail)
{
    Q_UNUSED(mail)
    helpers::log("off365:handleMail SHOULD NEVER HAPPEN", LOG_ERR, qApp, 0);
}

void CollectMailsOffice365::cleanupFiles(QString sPrefix)
{
    QString sDir = qApp->applicationDirPath()+QDir::separator()+"maildata";
    {
        QDir appDir(qApp->applicationDirPath());
        appDir.mkdir("maildata");
    }
    int iDeletedFiles = helpers::emptydir(sDir, qApp, 0, sPrefix, false);
    helpers::log("off365:cleanup("+sPrefix+"): deleted:"+QString::number(iDeletedFiles), LOG_INF, qApp, 0);
}

QString CollectMailsOffice365::getFN(QString sPrefix, QString sPostfix)
{
    sPrefix  = sPrefix.toLower().replace(QRegExp("[^\\d\\w]"),"_");
    sPostfix = sPostfix.toLower().replace(QRegExp("[^\\d\\w]"),"_");

    QString sDir = qApp->applicationDirPath()+QDir::separator()+"maildata";

    return sDir + QDir::separator() + sPrefix + sPostfix;
}
QString CollectMailsOffice365::prettify(QString s)
{
    s = s.replace("  " , " ")
         .replace("\\r", " ")
         .replace("\\n", " ")
         .replace("\\t", " ")
         .replace("\r" , " ")
         .replace("\n" , " ")
         .replace("\t" , " ")
         .replace("  " , " ")
         .replace("  " , " ").trimmed();

    if(s.compare("n/a", Qt::CaseInsensitive) == 0)
        return "0";

    return s;
}
