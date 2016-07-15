#include "headlesswebpagelogger.h"
#include <QTimer>
#include <QWebFrame>
#include <QNetworkReply>
#include "webpage.h"
#include "helpers.h"
#include "urlinfo.h"

HeadlessWebPageLogger::HeadlessWebPageLogger(QObject *parent) : WebPage(true, parent)
{

}

void HeadlessWebPageLogger::load(QString sUrl)
{
    connect(this->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    WebPage::beforeLoadUrl(this, sUrl);
    this->mainFrame()->load(QUrl(sUrl));
}

void HeadlessWebPageLogger::onLoadFinished(bool bOK)
{
    helpers::log(QString(" loadFinished:")+this->mainFrame()->url().toString(), bOK ? LOG_INF : LOG_WRN, qApp, 0);

    if(!bOK)
    {
        helpers::log(   "----------------------------------------------\n"+
                         this->mainFrame()->toHtml().left(512) +
                         "\n---------------------------------------------"
                         , bOK ? LOG_INF : LOG_WRN, qApp, 0);
    }
}

void HeadlessWebPageLogger::onHttpResponseFinished(QNetworkReply* reply)
{
    return;
    QString failedUrl           = reply->request().url().toString();
    int httpStatus              = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString httpStatusMessage   = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    helpers::log(QString("onHttpResponseFinished:")+
                 " urlinmainfr:"+this->mainFrame()->url().toString()+
                 " url:"+failedUrl+
                 " httpStatus:"+QString::number(httpStatus)+
                 " httpStatusMessage:"+httpStatusMessage
               , LOG_INF, qApp, 0);
}
