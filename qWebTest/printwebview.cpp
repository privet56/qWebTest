#include "printwebview.h"
#include "helpers.h"
#include <QWebInspector>
#include <QNetworkReply>
#include <QWebFrame>
#include <QWebSecurityOrigin>
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QWebInspector>
#include <QNetworkProxyFactory>
#include <QSize>
#include <QImage>
#include <QFile>
#include <QPainter>

PrintWebView::PrintWebView(QWidget *parent) : WebView(parent)
{

}

void PrintWebView::print(QUrl url)
{
    connect(this->page()->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    this->load(url);
}

void PrintWebView::onLoadFinished(bool bOK)
{
    disconnect(this->page()->mainFrame(), SLOT(loadFinished(bool)));

    if(!bOK)
    {
        helpers::log(QString(" loadFinished-withError ")+this->page()->mainFrame()->url().toString(), bOK ? LOG_INF : LOG_WRN, qApp, 0);
        return;
    }
    web2pic();
}
