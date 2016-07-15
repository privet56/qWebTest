#include "urldownloaderwp.h"
#include <QNetworkProxyFactory>
#include "helpers.h"

urldownloaderwp::urldownloaderwp(QObject *parent, QString url, QString un, QString pwd) : WebPage(true, parent)
{
    connect(this->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

    WebPage::beforeLoadUrl(this, url);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QUrl u(url, QUrl::StrictMode);
    if(!helpers::str_isempty(un, true)) u.setUserName(un);
    if(!helpers::str_isempty(pwd,true)) u.setPassword(pwd);
    this->mainFrame()->load(u);

    m_loop.exec();
}

void urldownloaderwp::onLoadFinished(bool bOK)
{
    if(!bOK)
    {
        helpers::log("urldwp:!ok "+this->mainFrame()->toPlainText(), LOG_WRN, qApp, 0);
    }
    //else
    {
        this->m_sResp = this->mainFrame()->toPlainText();
    }
    m_loop.quit();
}

QString urldownloaderwp::load(QObject *parent, QString url, QString un, QString pwd)
{
    Q_UNUSED(parent)
    urldownloaderwp* pud = new urldownloaderwp(0, url, un, pwd);
    QString s = pud->m_sResp;
    delete pud; pud = 0;
    return s;
}
