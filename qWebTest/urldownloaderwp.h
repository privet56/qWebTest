#ifndef urldownloaderwp_H
#define urldownloaderwp_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QHttpPart>
#include <QUrl>
#include <QSslConfiguration>
#include <QTimer>
#include <QTimer>
#include <QWebFrame>
#include <QNetworkReply>
#include "webpage.h"
#include "helpers.h"
#include "urlinfo.h"

class urldownloaderwp : public WebPage
{
    Q_OBJECT
public:
    explicit urldownloaderwp(QObject *parent, QString url, QString un, QString pwd);
    QString m_sResp;
    QEventLoop m_loop;

    static QString load(QObject *parent, QString url, QString un, QString pwd);

signals:

public slots:
    void onLoadFinished(bool bOK);

};

#endif // urldownloaderwp_H
