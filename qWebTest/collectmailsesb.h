#ifndef COLLECTMAILSESB_H
#define COLLECTMAILSESB_H

#include <QObject>
#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QNetworkAccessManager>
#include "xml.h"
#include <QtXml>
#include <QDomDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "helpers.h"
#include "collectmailsoffice365.h"

class collectmailsesb : public CollectMailsOffice365
{
    Q_OBJECT
public:
    explicit collectmailsesb(QObject *parent);

    void collect();
    virtual void handleMail(QJsonObject mail);
    void handleMailInfo();
    void handleMailTable(QString h5, QDomElement eTable, QJsonObject* pDay);

    QString getBetween(QString s, QString sStart, QString sEnd);

    QMap<QString, QJsonArray*> mailInfos;

    static void __testjson();

signals:

public slots:

};

#endif // COLLECTMAILSESB_H
