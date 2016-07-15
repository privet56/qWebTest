#include "collectmailsesb.h"
#include "xml.h"
#include <QtXml>
#include <QDomDocument>
#include <QJsonArray>

collectmailsesb::collectmailsesb(QObject *parent) : CollectMailsOffice365(parent, 0)
{

}
void collectmailsesb::collect()
{
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdReceived = getFolderId("", "received");
    if(helpers::str_isempty(sIdReceived, true))
    {
        helpers::log("cmailsesb:collect r1 !received", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdSenSoaSS = getFolderId(sIdReceived, "sen-soa-sales-services");
    if(helpers::str_isempty(sIdSenSoaSS, true))
    {
        helpers::log("cmailsesb:collect r2 !sen-soa-sales-services", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    QString sIdEsb    = getFolderId(sIdSenSoaSS, "esb");
    if(helpers::str_isempty(sIdEsb, true))
    {
        helpers::log("cmailsesb:collect r3 !esb", LOG_WRN, qApp, 0);
        return;
    }
helpers::log(".", LOG_INF, qApp, 0);
    collectMailInfo(sIdEsb, "", 0);
helpers::log(".", LOG_INF, qApp, 0);

    handleMailInfo();
}

void collectmailsesb::handleMailInfo()
{
    cleanupFiles("esb_*");

    QJsonDocument doc;

    QJsonObject data;
    QJsonObject datacontent;

    QMapIterator<QString, QJsonArray*> iMailsInfos(mailInfos);
    int _iMailsInfos = 0;
    while (iMailsInfos.hasNext())
    {
        //_iMailsInfos++;
        iMailsInfos.next();
        QString sSubject = iMailsInfos.key();
        QJsonArray* lMails = iMailsInfos.value();
        datacontent.insert(sSubject, QJsonValue(*lMails));
        _iMailsInfos += lMails->size();
    }

    data.insert("data", QJsonValue(datacontent));
    doc.setObject(data);

    QFile file(QString(getFN("esb_", "maildata")+".json"));
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(doc.toJson(QJsonDocument::Compact));
    file.close();

    //TODO: delete QJsonArray*'s

    helpers::log("cmesb:handleMailInfo(esb): FINISH -> "+QString::number(_iMailsInfos), LOG_INF, qApp, 0);
}
void collectmailsesb::handleMail(QJsonObject mail)
{
    QString sSubject = mail["Subject"].toString();
    if(helpers::str_isempty(sSubject, true))return;
    if(sSubject.indexOf('[') != 0)return;
    if(sSubject.indexOf(" ESB Notification") < 3)return;

    //[SOA-SALES-SERVICES Prod] [Cluster B] ESB Notification
    //[SEN-SOA-SALES-SERVICES Production] ESB Notification
    //[SOA-SALES-SERVICES] ESB Notification (TEST-Environment)

    sSubject = prettify(sSubject.replace("[UNIFY-", "[").replace("[SEN-", "["));

    //[SOA-SALES-SERVICES Prod] [Cluster B] ESB Notification
    //[SOA-SALES-SERVICES Production] ESB Notification
    //[SOA-SALES-SERVICES] ESB Notification (TEST-Environment)

    QJsonObject body = mail["Body"].toObject();
    QString sBody = body["Content"].toString();
    if(helpers::str_isempty(sBody, true))
    {
        helpers::log("cmailsesb:handleMail !sBody:'"+sSubject+"'", LOG_WRN, qApp, 0);
        return;
    }
    sBody = sBody.replace("<br>", "\n").replace("\\r\\n", "\n").replace("\\n\\n", "\n").trimmed().replace(".\n", "\n").replace("\\\"", "\"");
    //helpers::log("cmailsesb:handleMail sBody:'"+sSubject+"'----------\n"+sBody+"\n------------------------", LOG_INF, qApp, 0);

    //get html between <body ... </body>
    sBody = getBetween(sBody, "<body","</body>");
    {   //remove <hr ...>
        static QRegExp hr("<hr(.)[^>]*>");
        sBody = sBody.remove(hr);
    }

    QDomDocument doc(xml::GetRootEle(sBody));
    QString errorStr;
    int errorLine=0, errorColumn=0;
    if (!doc.setContent(sBody, true, &errorStr, &errorLine, &errorColumn))
    {
        helpers::log("cmailsesb:handleMail !xml:'"+sSubject+"'-----------------\n"+sBody+"\n----------------------", LOG_WRN, qApp, 0);
        return;
    }
    QDomElement root = doc.documentElement();
    QJsonObject day;
    {
        QDomElement e = root.firstChildElement();
        QString h5 = "";
        while (!e.isNull())
        {
            QString sTagName = e.nodeName().toLower().trimmed();
            if(sTagName.compare("h5", Qt::CaseInsensitive) == 0)
            {
                h5 = prettify(xml::getText(e));
            }
            else if(sTagName.compare("table", Qt::CaseInsensitive) == 0)
            {
                handleMailTable(h5, e, &day);
                h5 = "";
            }

            e = e.nextSiblingElement();
        }
    }
    {
        QString sDate = mail["DateTimeSent"].toString();
        day.insert("date", QJsonValue(sDate));
    }

    QJsonArray* pa = mailInfos[sSubject];
    if(!pa)
    {
        pa = new QJsonArray();
        mailInfos[sSubject] = pa;
    }
    pa->append(QJsonValue(day));
}
void collectmailsesb::handleMailTable(QString h5, QDomElement eTable, QJsonObject* pDay)
{
    if(helpers::str_isempty(h5, true))
    {
        helpers::log("cmailsesb:handleMailTable !h5:'"+h5+"'", LOG_WRN, qApp, 0);
        return;
    }

    QDomElement eTHeadTr = eTable.firstChildElement("thead").firstChildElement("tr");
    QDomNodeList leTHeadTds = eTHeadTr.elementsByTagName("th");

    QDomElement eTBody   = eTable.firstChildElement("tbody");
    QDomElement eTBodyTr = eTBody.firstChildElement("tr");

    QJsonArray a;

    while(!eTBodyTr.isNull())
    {
        QDomNodeList leTBodyTds = eTBodyTr.elementsByTagName("td");

        if(leTHeadTds.size() == leTBodyTds.size())
        {
            QJsonObject service;
            for(int i=0;i<leTHeadTds.size();i++)
            {
                QString sVal = prettify(leTBodyTds.at(i).firstChild().nodeValue().trimmed());
                QString sKey = prettify(leTHeadTds.at(i).firstChild().nodeValue());

                if(helpers::str_isnum(sVal, true, true))
                {
                    if(sVal.indexOf('.') > 0)
                    {
                        service.insert(sKey, QJsonValue(sVal.toDouble()));
                    }
                    else
                    {
                        service.insert(sKey, QJsonValue(sVal.toInt()));
                    }
                }
                else
                {
                    service.insert(sKey, QJsonValue(sVal));
                }
            }

            a.append(service);
        }
        else
        {
            helpers::log("cmailsesb:handleMailTable step3: head <body>:"+QString::number((leTHeadTds.size()))+" != "+QString::number((leTBodyTds.size())), LOG_WRN, qApp, 0);
        }

        eTBodyTr = eTBodyTr.nextSiblingElement("tr");
    }
    pDay->insert(h5, QJsonValue(a));
}
QString collectmailsesb::getBetween(QString s, QString sStart, QString sEnd)
{
    int iStart  = s.indexOf(sStart);
    int iEnd    = s.indexOf(sEnd, iStart);

    if((iStart > -1) && (iEnd > iStart))
    {
        return s.mid(iStart, iEnd - iStart + sEnd.size());
    }

    helpers::log("cmailsesb:getBetween ? start::"+QString::number(iStart)+" end:"+QString::number(iEnd)+"-----------------\n"+s+"\n----------------------", LOG_WRN, qApp, 0);
    return s;
}

void collectmailsesb::__testjson()
{
    QJsonDocument doc;
    QJsonObject data;
    data.insert("data", QJsonValue(QJsonObject()));
    {
        //QJsonObject esb;
        //esb.insert("[ESB]", QJsonValue(QJsonArray()));

        QJsonArray a;
        a.append(QJsonValue(QString("ae")));
        data.insert("data2", QJsonValue(a));

        QJsonArray aa = data.value("data2").toArray();
        aa.append(QJsonValue(QString("aee")));
        data.remove("data2");
        data.insert("data2", QJsonValue(aa));

        //doc.object().value("data").toObject().insert("[ESB]", QJsonValue(a));
        //QJsonObject data2;
        //data2.insert("data2", QJsonValue(QJsonObject()));

        //data.insert("data2", QJsonValue(QString("val2")));
    }
    doc.setObject(data);

    QMessageBox::warning(0, "__testjson", doc.toJson());

    /*
    QFile file("c:/temp/test.json");
    file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    file.write(doc.toJson());
    file.close();
    */
}

//http://research2guidance.com/only-5-of-mobile-app-developers-who-use-cross-platform-tools-say-native-app-development-brings-more-value-formoney/#content
