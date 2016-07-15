#include <QWebFrame>
#include <QNetworkProxyFactory>
#include "helpers.h"
#include "headlessloadtest.h"
#include "headlesswebpage.h"

HeadlessLoadTest::HeadlessLoadTest(QObject *parent) : QObject(parent)
{
}

void HeadlessLoadTest::loadtest()
{
    int iMax = helpers::GetCFG_int("loadtest_instances", qApp, 0);
    if (iMax < 1)
        iMax = 3;

    QString sUrl = helpers::GetCFG("loadtest_urls", qApp, 0).toLower().trimmed();
    if (sUrl.isNull() || sUrl.isEmpty())
    {
        helpers::log("no url -> no loadtest", LOG_INF, qApp, 0);
        return;
    }

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QStringList urls = sUrl.split("|", QString::SkipEmptyParts, Qt::CaseInsensitive);

    m_pagesFinished.empty();

    for(int i=0; i < iMax; i++)
    {
        HeadlessWebPage* pp = new HeadlessWebPage(i, this);
        m_pagesWorking.insert(pp);
        pp->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        pp->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
        pp->load(urls);
        connect(pp, SIGNAL(testFinished(HeadlessWebPage*)), this, SLOT(testFinished(HeadlessWebPage*)));
    }
    helpers::log("START "+QString::number(iMax)+" instances of urls("+QString::number(urls.size())+"):"+sUrl, LOG_INF, qApp, 0);
}

void HeadlessLoadTest::testFinished(HeadlessWebPage* hwp)
{
    QMutexLocker locker(&mutex);    //synchronized

    if(m_pagesWorking.isEmpty())return;

    hwp->triggerAction(QWebPage::Stop);
    m_pagesWorking.remove(hwp);
    m_pagesFinished.insert(hwp);

    int iErrors=0;
    {
        QSetIterator<HeadlessWebPage*> i(m_pagesFinished);
        while (i.hasNext())
        {
            HeadlessWebPage* hwp = i.next();
            if(hwp->m_urlsWithError.size() > 0)
                iErrors++;
        }
    }

    if((m_pagesFinished.size() % 100 == 0) || m_pagesWorking.isEmpty())
    {
        helpers::log(QString(m_pagesWorking.isEmpty() ? " FINISH " : "") +
                    "iLoadFinished:"+QString::number(m_pagesFinished.size())+"/"+QString::number(m_pagesFinished.size() + m_pagesWorking.size())+
                     " Errors:"+QString::number(iErrors)
                     , LOG_INF, qApp, 0);

        if((iErrors > 0) && m_pagesWorking.isEmpty())
        {
            QSetIterator<HeadlessWebPage*> ipf(m_pagesFinished);
            while (ipf.hasNext())
            {
                HeadlessWebPage* hwp = ipf.next();
                for (int i = 0; i < hwp->m_urlsWithError.size(); ++i)
                {
                    helpers::log(" pageIdx:"+QString::number(hwp->m_iIndex)+
                                 " errorIdx:"+QString::number(i)+
                                 " testUIdx:"+QString::number(hwp->m_urlsWithError.at(i)->m_iTestIndex)+
                                 " urlWithError: "+hwp->m_urlsWithError.at(i)->m_url
                                 , LOG_INF, qApp, 0);
                }
            }
        }
    }
}
