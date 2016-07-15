#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include "helpers.h"
#include <QUrl>
#include <QDir>
#include <QSet>
#include <QFile>
#include <QFileInfo>
#include <QString>

class networkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    networkAccessManager(QNetworkAccessManager *oldManager, QObject *parent = 0);
    ~networkAccessManager();
protected:
    QNetworkReply* createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *device);
public slots:
    void onRequestMetaDataChanged();

};

#endif // NETWORKACCESSMANAGER_H
