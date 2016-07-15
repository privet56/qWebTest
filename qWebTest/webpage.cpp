#include <QWebInspector>
#include <QNetworkReply>
#include <QWebFrame>
#include <QWebSecurityOrigin>
#include "webpage.h"
#include "helpers.h"

WebPage::WebPage(bool bSilent, QObject *parent) : QWebPage(parent)
{
    if(!bSilent)
    {
        setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
        this->setForwardUnsupportedContent(true);

        QWebInspector *inspector = new QWebInspector();
        inspector->setPage(this);
    }
    else
    {
        this->setForwardUnsupportedContent(false);
    }

    connect(this->networkAccessManager(), SIGNAL(finished(QNetworkReply *)), this, SLOT(httpResponseFinished(QNetworkReply *)));
}
bool WebPage::acceptNavigationRequest ( QWebFrame * frame, const QNetworkRequest & request, NavigationType type )
{
    //if(type == QWebPage::NavigationTypeLinkClicked)QMessageBox::information(0, "acceptNavigationRequest","NavigationTypeLinkClicked", 1,0,0);
    QWebPage::acceptNavigationRequest(frame, request, type);
    return true;
}
void WebPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID )
{
    QWebPage::javaScriptConsoleMessage(message, lineNumber, sourceID);
    qDebug() << "jsconsolemsg: "+message;
}

void WebPage::httpResponseFinished(QNetworkReply * reply)
{
    onHttpResponseFinished(reply);
}
void WebPage::onHttpResponseFinished(QNetworkReply* reply)
{
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            return;
        }
        case QNetworkReply::OperationCanceledError:
        {
            return;
        }
        case QNetworkReply::ContentNotFoundError:       // 404 Not found
        default:
        {
            QString failedUrl           = reply->request().url().toString();
            int errorCode1              = reply->error();
            int httpStatus              = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QString httpStatusMessage   = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

            helpers::log("httpError: urlinmainfr:"+this->mainFrame()->url().toString()+
                       " errorCode:"+QString::number(errorCode1)+
                       " failedUrl:"+failedUrl+
                       " httpStatus:"+QString::number(httpStatus)+
                       " httpStatusMessage:"+httpStatusMessage
                       , LOG_WRN, qApp, 0);
            return;
        }
    }
}

void WebPage::beforeLoadUrl(QWebPage* page, QString sUrl)
{
    if(helpers::str_isempty(sUrl, true))return;
    sUrl = sUrl.replace(QString("\\"), QString("/"));
    if(sUrl.toLower().startsWith("file:") ||
       sUrl.startsWith("//") ||
       (sUrl.length() > 1) && (sUrl.at(1) == ':'))
    {
        //local file
        return;
    }

    sUrl = sUrl.replace(QString("http://"), QString(""), Qt::CaseInsensitive);
    sUrl = sUrl.replace(QString("https://"), QString(""), Qt::CaseInsensitive);

    int iSlash = sUrl.indexOf('/');
    if( iSlash > 1)
        sUrl = sUrl.left(iSlash);

    page->mainFrame()->securityOrigin().addAccessWhitelistEntry("http" , sUrl, QWebSecurityOrigin::AllowSubdomains);
    page->mainFrame()->securityOrigin().addAccessWhitelistEntry("https", sUrl, QWebSecurityOrigin::AllowSubdomains);
}
