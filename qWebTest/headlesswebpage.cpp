#include <QDebug>
#include <QWebFrame>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "headlesswebpage.h"
#include "helpers.h"

#define NEXTTEST_TIMEOUT 500

HeadlessWebPage::HeadlessWebPage(int iIndex, QObject *parent) : WebPage(true, parent), timer(this)
{
    m_iIndex = iIndex;
    m_bLoadFinished = false;
    m_bCheckLoadFinished = true;
    this->m_urls_done_index = -1;
}

void HeadlessWebPage::load(QStringList urls)
{
    m_urlsWithError.empty();
    this->m_urls.empty();
    this->m_urls << urls;

    m_bLoadFinished = false;
    this->m_urls_done_index = -1;

    //connect(this, SIGNAL(loadStarted())   , this, SLOT(onLoadStarted()));
    //connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    connect(this->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    nextTest();
}

void HeadlessWebPage::onLoadStarted()
{
    m_bLoadFinished = false;
}

void HeadlessWebPage::onTimeOut()
{
    if(m_bLoadFinished)
    {
        timer.stop();
        return;
    }
    this->onLoadFinished(false, true);
}

void HeadlessWebPage::onLoadFinished(bool bOK)
{
    this->onLoadFinished(bOK, false);
}

void HeadlessWebPage::onLoadFinished(bool bOK, bool bTimeOut)
{
    this->timer.stop();
    //this->triggerAction(QWebPage::Stop);
    //this->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);

    if(m_bLoadFinished)
    {
        helpers::log("pageidx:"+QString::number(m_iIndex) + " timeOut:" + QString::number(bTimeOut) + " m_bLoadFinished urlsdone:"+ QString::number(this->m_urls_done_index)+ " " +this->mainFrame()->url().toString(), LOG_WRN, qApp, 0);
        return;
    }

    if(!this->m_bCheckLoadFinished)
    {
        this->m_bCheckLoadFinished = true;
        QTimer::singleShot(NEXTTEST_TIMEOUT, this, SLOT(nextTest()));
        return;
    }

    if(this->mainFrame()->url().toString() == "about:blank")
    {
        QTimer::singleShot(NEXTTEST_TIMEOUT*10, this, SLOT(nextTest()));
        helpers::log("pageidx:"+QString::number(m_iIndex) + " timeOut:" + QString::number(bTimeOut) + " about:blank urlsdone:"+ QString::number(this->m_urls_done_index)+ " " +this->mainFrame()->url().toString(), LOG_WRN, qApp, 0);
        return;
    }

    this->m_urls_done_index++;

    if(!bOK)
    {
        m_urlsWithError.append(new UrlInfo(m_urls.at(this->m_urls_done_index)+" ("+this->mainFrame()->url().toString()+")", this->mainFrame()->toHtml(), this->m_urls_done_index, this));
        helpers::log("pageidx:"+QString::number(m_iIndex) + " timeOut:" + QString::number(bTimeOut) + QString(" loadFinished-withError ")+this->mainFrame()->url().toString(), bOK ? LOG_INF : LOG_WRN, qApp, 0);

        helpers::log(   "----------------------------------------------\n"+
                         this->mainFrame()->toHtml().left(512)+
                         "\n---------------------------------------------"
                         , bOK ? LOG_INF : LOG_WRN, qApp, 0);
    }

    if((this->m_urls.size() - 1) <= this->m_urls_done_index)
    {
        disconnect(this, SLOT(onLoadFinished(bool)));
        disconnect(this->mainFrame(), SLOT(onLoadFinished(bool)));
        m_bLoadFinished = true;
        helpers::log("*", bOK ? LOG_INF : LOG_WRN, qApp, 0, false);
        emit testFinished(this);
        return;
    }

    helpers::log(".", bOK ? LOG_INF : LOG_WRN, qApp, 0, false);

    this->m_bCheckLoadFinished = false;
    QTimer::singleShot(NEXTTEST_TIMEOUT, this, SLOT(setEmpty()));
}

void HeadlessWebPage::setEmpty()
{
    if(this->m_bLoadFinished)return;

    this->m_bCheckLoadFinished = false;
    this->mainFrame()->setHtml(" ", QUrl("about:blank"));
}

void HeadlessWebPage::nextTest()
{
    if(this->m_bLoadFinished)return;

    this->m_bCheckLoadFinished = true;

    int idx = this->m_urls_done_index + 1;
    if (idx > (m_urls.size() - 1))
    {
        helpers::log("miscalculated: pageidx:"+QString::number(m_iIndex)+
                     " urlsSize:"+QString::number(m_urls.size())+
                     " m_urls_done_index:"+QString::number(this->m_urls_done_index)
                     , LOG_WRN, qApp, 0);

        this->m_urls_done_index = (m_urls.size() - 2);
        onLoadFinished(false, false);
        return;
    }

    QString sUrl = this->m_urls.at(this->m_urls_done_index + 1);
    //this->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);    //it crashes if line activated
    WebPage::beforeLoadUrl(this, sUrl);
    this->mainFrame()->load(QUrl(sUrl));
    timer.start(1000 * 60); //=(re)start
}

void HeadlessWebPage::onHttpResponseFinished(QNetworkReply* reply)
{
    if(this->m_bLoadFinished)return;

    WebPage::onHttpResponseFinished(reply);
    /*
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            return;
        }
        case QNetworkReply::ContentNotFoundError:       // 404 Not found
        default:
        {
            QString failedUrl           = reply->request().url().toString();
            int httpStatus              = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QString httpStatusMessage   = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

            helpers::log("httpError: pageidx:"+QString::number(m_iIndex)+
                       " urlsSize:"+QString::number(m_urls.size())+
                       " url2load:"+getCurrentUrl2Load()+
                       " m_urls_done_index:"+QString::number(this->m_urls_done_index)+
                       " urlinmainfr:"+this->mainFrame()->url().toString()+
                       " failedUrl:"+failedUrl+
                       " httpStatus:"+QString::number(httpStatus)+
                       " httpStatusMessage:"+httpStatusMessage
                       , LOG_WRN, qApp, 0);
            return;
        }
    }
    */
}

QString HeadlessWebPage::getCurrentUrl2Load()
{
    if(!this->m_bCheckLoadFinished) return "about:blank?already-finished";

    int idx = this->m_urls_done_index + 1;
    if (idx > (m_urls.size() - 1))
    {
        return "about:blank?miscalculated&idx="+QString::number(idx)+"&urlsSize="+QString::number(m_urls.size());
    }
    return this->m_urls.at(this->m_urls_done_index + 1);
}
