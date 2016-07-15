#include "networkaccessmanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QWebView>
#include "webpage.h"
#include <QThread>
#include "networkReply.h"
#include "helpers.h"

networkAccessManager::networkAccessManager(QNetworkAccessManager *oldManager, QObject *parent /*= 0*/) : QNetworkAccessManager(parent)
{
    setCache(oldManager->cache());
    setCookieJar(oldManager->cookieJar());
    setProxy(oldManager->proxy());
    setProxyFactory(oldManager->proxyFactory());
}

QNetworkReply* networkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *device)
{
    //helpers::log("op:"+QString::number(op) + " url:" + req.url().toString(), LOG_INF, qApp, 0);

    {
        QWebView* wv = dynamic_cast<QWebView*>(this->parent());
        if(wv != 0)
        {
            WebPage::beforeLoadUrl(wv->page(), req.url().toString());
        }
    }

    /*if(op == QNetworkAccessManager::GetOperation)
    {
        if(req.url().scheme() == "file")
        {
           networkReply* nr = new networkReply(0, req, op);
           nr->deleteLater();
           return nr;
        }
    }*/

    QNetworkReply* nr = QNetworkAccessManager::createRequest(op, req, device);

    if(op == QNetworkAccessManager::HeadOperation)
    {
        //connect(nr, SIGNAL(metaDataChanged()), this, SLOT(onRequestMetaDataChanged()));
        //TODO:
        /*
        networkReply* nr2 = new networkReply(0, nr);
        connect(nr, SIGNAL(metaDataChanged()), this, SLOT(onRequestMetaDataChanged()));
        nr2->deleteLater();
        nr = nr2;
        */
    }

    return nr;
}

networkAccessManager::~networkAccessManager()
{

}

void networkAccessManager::onRequestMetaDataChanged()
{
    return;
    QNetworkReply* nr = dynamic_cast<QNetworkReply*>(sender());
    if(nr != NULL && (nr->operation() == QNetworkAccessManager::HeadOperation))
    {
        nr->write("Access-Control-Allow-Origin:*\r\n");
        nr->write("Access-Control-Allow-Origin:*\r\n");
        nr->write("Access-Control-Allow-Headers:Content-Type, Authorization, Accept\r\n");
        nr->write("Access-Control-Allow-Headers:X-Requested-With\r\n");
        nr->write("Access-Control-Allow-Methods:GET, POST, OPTIONS, HEAD\r\n");
    }
}
