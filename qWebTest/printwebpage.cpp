#include "printwebpage.h"
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

PrintWebPage::PrintWebPage(QObject *parent) : WebPage(true, parent)
{

}

void PrintWebPage::print(QUrl url)
{
    connect(this->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    this->mainFrame()->load(url);
}

void PrintWebPage::onLoadFinished(bool bOK)
{
    disconnect(this, SLOT(onLoadFinished(bool)));
    disconnect(this->mainFrame(), SLOT(loadFinished(bool)));

    if(!bOK)
    {
        helpers::log(QString(" loadFinished-withError ")+this->mainFrame()->url().toString(), bOK ? LOG_INF : LOG_WRN, qApp, 0);
        return;
    }
    web2pic();
}

QString PrintWebPage::web2pic()
{
    QWebFrame *frame = this->mainFrame();
    QSize size       = frame->contentsSize();
    QImage* image = new QImage(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied/*QImage::Format_RGB32*/);

    /*{
        QSize vp(size);
        vp.setHeight(size.height()*2);
        vp.setWidth(size.width()*2);
        page->setViewportSize(vp);
    }*/

    image->fill(Qt::transparent);

    image->fill(QColor(Qt::green).rgb());

    QPainter* painter = new QPainter(image);

    painter->setRenderHint(QPainter::Antialiasing, true);       //http://qt.gitorious.org/qt-labs/graphics-dojo/blobs/master/webcapture/webcapture.cpp
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->setBackground(QBrush(Qt::white));
    painter->setRenderHints(QPainter::Antialiasing);
    frame->render(painter);
    painter->end();
    if(painter)delete painter;painter=0;
    QString sFN = qApp->applicationDirPath()+"/web2pic";
    int i=1;
    while(QFile::exists(sFN+QString::number(i)+".png"))
    {
        i++;
    }
    if(!image->save(sFN+QString::number(i)+".png"))
    {
        helpers::log("!img.save !ok "+sFN+QString::number(i)+".png", LOG_ERR, qApp, 0);
        return QString::null;
    }
    helpers::log("img.save "+sFN+QString::number(i)+".png", LOG_INF, qApp, 0);
    return sFN+QString::number(i)+".png";
}
