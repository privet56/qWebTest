#ifndef COLLECTMAILSOFFICE365_H
#define COLLECTMAILSOFFICE365_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QMap>
#include <QList>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "helpers.h"

class CollectMailsOffice365 : public QObject
{
    Q_OBJECT
public:
    explicit CollectMailsOffice365(QObject *parent, QNetworkAccessManager* pMan);
    QString getFolderId(QString sFolderId, QString sFolderName2Return);
    QString getId(QString sJson, QString sFolderName);
    QString getUrl(QString sFolderId, QString sSubFolder, QString sUrl);

    void collectMailInfo(QString sFolderIdMongo, QString sNextPageFullUrl, int iPage);
    virtual void handleMail(QJsonObject mail);

    void cleanupFiles(QString sPrefix);
    QString getFN(QString sPrefix, QString sPostfix);

    QString prettify(QString s);

    QNetworkAccessManager* m_pMan;

signals:

public slots:

};

#endif // COLLECTMAILSOFFICE365_H
