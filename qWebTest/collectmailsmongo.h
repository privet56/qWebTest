#ifndef COLLECTMAILSMONGO_H
#define COLLECTMAILSMONGO_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QNetworkAccessManager>
#include "helpers.h"
#include "collectmailsoffice365.h"

class CollectMailsMongo : public CollectMailsOffice365
{
    Q_OBJECT
public:
    explicit CollectMailsMongo(QObject *parent, QNetworkAccessManager* pMan);

    void collect();
    virtual void handleMail(QJsonObject mail);
    void handleMailInfo();

    QMap<QString, QList<QMap<QString, QString>*>*> mailInfos;

signals:

public slots:

};

#endif // COLLECTMAILSMONGO_H
