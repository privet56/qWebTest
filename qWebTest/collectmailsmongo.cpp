#include "collectmailsmongo.h"

CollectMailsMongo::CollectMailsMongo(QObject *parent, QNetworkAccessManager* pMan) : CollectMailsOffice365(parent, pMan)
{

}
void CollectMailsMongo::collect()
{
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdReceived = getFolderId("", "received");
    if(helpers::str_isempty(sIdReceived, true))
    {
        helpers::log("cmailsMongo:collect r1 !received", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdSenSoaSS = getFolderId(sIdReceived, "sen-soa-sales-services");
    if(helpers::str_isempty(sIdSenSoaSS, true))
    {
        helpers::log("cmailsMongo:collect r2 !sen-soa-sales-services", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdMongo    = getFolderId(sIdSenSoaSS, "mongo");
    if(helpers::str_isempty(sIdMongo, true))
    {
        helpers::log("cmailsMongo:collect r3 !mongo", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    collectMailInfo(sIdMongo, "", 0);
helpers::log(".", LOG_INF, qApp, 0);
    handleMailInfo();
}

void CollectMailsMongo::handleMailInfo()
{
    QMapIterator<QString, QList<QMap<QString, QString>*>*> iMailsInfos(mailInfos);
    cleanupFiles("mongo_*");

    FILE* pFjsn = fopen(QString(getFN("mongo_", "maildata")+".json").toLatin1(), "a+"/*append*/);
    fputs(QString("{\"data\":{\n").toLatin1(),pFjsn);

    int _iMailsInfos = 0;
    while (iMailsInfos.hasNext())
    {
        _iMailsInfos++;
        iMailsInfos.next();
        QString sSubject = iMailsInfos.key();
        QList<QMap<QString, QString>*>* lMails = iMailsInfos.value();
        if(lMails->size() < 1)
        {
            helpers::log("cmailsMongo:handleMailInfo !mails 4 sub:'"+sSubject+"'", LOG_WRN, qApp, 0);
            continue;
        }

        QString sFN = getFN("mongo_", sSubject);
helpers::log("cmMongo:handleMailInfo: '"+sSubject+"' -> "+QString::number(lMails->size()), LOG_INF, qApp, 0);
        FILE* pFcsv = fopen(QString(sFN+".csv" ).toLatin1(), "a+"/*append*/);
        {   //csv header
            QMap<QString, QString>* mailEntries = lMails->at(0);
            QMapIterator<QString, QString> iMailEntries(*mailEntries);
            while(iMailEntries.hasNext())
            {
                iMailEntries.next();
                fputs(QString("\""+iMailEntries.key()+"\"\t").toLatin1(),pFcsv);
            }
        }
        {   //json starter
            //bool bFirstMailInfo = (_iMailsInfos < 1);
            fputs(QString("\""+sSubject+"\":[\n").toLatin1(),pFjsn);
        }
        for(int i=0;i<lMails->size();i++)
        {
            fputs(QString("\n").toLatin1(),pFcsv);
            fputs(QString("\n{").toLatin1(),pFjsn);

            QMap<QString, QString>* mailEntries = lMails->at(i);
            QMapIterator<QString, QString> iMailEntries(*mailEntries);

            int iEntry = 0;
            while(iMailEntries.hasNext())
            {
                iEntry++;
                iMailEntries.next();

                bool bLastEntry  = iEntry >= mailEntries->size();
                QString sVal     = iMailEntries.value();
                bool bIsNum      = helpers::str_isnum(sVal, true, true);

                if(!bIsNum)sVal = "\""+sVal+"\"";

                {
                    QString sTab = bLastEntry ? "" : "\t";
                    sVal = sVal.replace("\t", " ");
                    fputs(QString(sVal+sTab).toLatin1(),pFcsv);
                }
                {
                    QString sTab = bLastEntry ? "" : ", ";
                    sVal = sVal.replace(",", " ");
                    fputs(QString("\t\""+iMailEntries.key()+"\":"+sVal+sTab).toLatin1(),pFjsn);
                }
            }

            bool bLastEntry  = i >= (lMails->size()-1);
            QString sComma = bLastEntry ? "" : ",";
            fputs(QString("}\n"+sComma).toLatin1(),pFjsn);

        }   //end foreach lMails of a subject

        bool bLastEntry  = _iMailsInfos >= (mailInfos.size());
        QString sComma = bLastEntry ? "" : ",";
        fputs(QString("]\n"+sComma).toLatin1(),pFjsn);

        fclose(pFcsv);
    }
    fputs(QString("\n}}").toLatin1(),pFjsn);
    fclose(pFjsn);
helpers::log("cmMongo:handleMailInfo(mongo): FINISH -> "+QString::number(mailInfos.size()), LOG_INF, qApp, 0);
}
void CollectMailsMongo::handleMail(QJsonObject mail)
{
    QString sSubject = mail["Subject"].toString();
    if(helpers::str_isempty(sSubject, true))return;
    if(sSubject.indexOf('[') != 0)return;
    if(!sSubject.endsWith(" - INFO"))return;

    //[UNIFY-SOA-SALES-SERVICES mchp490a Mongo - clusterb] - INFO
    //[SOA-SALES-SERVICES mchp490a Mongo] - INFO
    //[SEN-SOA-SALES-SERVICES] Mongo - INFO
    //[SEN-SOA-SALES-SERVICES mchp490a] Mongo - INFO
    //[UNIFY-SOA-SALES-SERVICES mchp406a] Mongo TEST - INFO
    //[SEN-SOA-SALES-SERVICES mchp406a] Mongo TEST - INFO

    sSubject = sSubject .replace("[UNIFY-", "[")
                        .replace("[SEN-", "[")
                        .replace("-SERVICES] Mongo", "-SERVICES mchp490a] Mongo")
                        .replace(" Mongo]", "] Mongo")
                        .replace(" Mongo - clusterb]", "] Mongo - clusterb]");

    //[SOA-SALES-SERVICES mchp490a] Mongo - clusterb - INFO
    //[SOA-SALES-SERVICES mchp490a] Mongo - INFO
    //[SOA-SALES-SERVICES mchp406a] Mongo TEST - INFO

    sSubject = sSubject.trimmed();

    if(!sSubject.startsWith("[SOA-SALES-SERVICES mchp"))
    {
        helpers::log("cmailsMongo:handleMail !sSubject:'"+sSubject+"'", LOG_WRN, qApp, 0);
        return;
    }

    QJsonObject body = mail["Body"].toObject();
    QString sBody = body["Content"].toString();
    if(helpers::str_isempty(sBody, true))
    {
        helpers::log("cmailsMongo:handleMail !sBody:'"+sSubject+"'", LOG_WRN, qApp, 0);
        return;
    }
    sBody = sBody.replace("<br>", "\n").replace("\\r\\n", "\n").trimmed().replace(".\n", "\n").replace("\\\"", "\"");
    QStringList asBody = sBody.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    QMap<QString, QString>* pMailInfo = new QMap<QString, QString>();

    for(int i=0;i<asBody.size();i++)
    {
        QString sLine = asBody[i];
        int iD = sLine.indexOf(':');
        if (iD < 2)continue;
        QString sKey = sLine.left(iD).trimmed();
        QString sVal = sLine.mid(iD+1).trimmed();
        static QRegExp h("<(.)[^>]*>");
        sKey = sKey.remove(h);
        sVal = sVal.remove(h);
        if (sVal.endsWith('.'))
            sVal = sVal.left(sVal.size()-1);

        sVal = sVal.remove(".global-intra.net");

        if((sKey == "New active DB") ||                         //DB1 DB2
           (sKey == "Oracle2Mongo migration daily run") ||      //SUCCESS
           (sKey == "External request filename"))               //null
        {
            continue;
        }

        pMailInfo->insert(sKey, sVal);
    }

    QList<QMap<QString, QString>*>* paMailInfos4Subject = mailInfos[sSubject];
    if (paMailInfos4Subject == 0)
    {
        paMailInfos4Subject = new QList<QMap<QString, QString>*>();
        mailInfos[sSubject] = paMailInfos4Subject;
    }
    paMailInfos4Subject->operator <<(pMailInfo);
}

//https://outlook.office365.com/api/v1.0/me/folders/
//https://outlook.office365.com/api/v1.0/me/folders/AAMkAGJiOTM0MGNmLTg5OGEtNDEwNS1hNDJkLTBmODQ3ZDVkMjdmNQAuAAAAAAC-AvcG5C4zSYY1dWWgqjj9AQDanmAfvaaGSZqvDuER-gWmAAAGYYglAAA=/ChildFolders
