#include "webview.h"
#include "webpage.h"
#include "helpers.h"
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QWebInspector>
#include <QNetworkProxyFactory>
#include <QWebSecurityOrigin>
#include <QWebFrame>
#include <QSize>
#include <QImage>
#include <QFile>
#include <QPainter>


WebView::WebView(QWidget *parent) : QWebView(parent)
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavaEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LinksIncludedInFocusChain, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::XSSAuditingEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::NotificationsEnabled, true);
    QWebSettings::setIconDatabasePath(QDir::tempPath());//needed QWebView::iconChanged() to be fired!
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    //QWebSecurityOrigin::allOrigins().at(0).

    this->setPage(new WebPage(false, this));
}
QWebView* WebView::createWindow ( QWebPage::WebWindowType type )
{
    //type = QWebPage::WebBrowserWindow | QWebPage::WebModalDialog
    return new WebView(0);
    return QWebView::createWindow(type);
}

QString c2s(QRgb r)
{
    return "("+QString::number(qRed(r)) + ","+QString::number(qGreen(r))+","+QString::number(qBlue(r))+")";
}

QString WebView::web2pic()
{
    QWebPage *page   = this->page();
    QWebFrame *frame = page->mainFrame();
    QSize size       = frame->contentsSize();
    QImage* image = new QImage(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied/*QImage::Format_RGB32*/);

    QSize vp(size);
    page->setViewportSize(vp);

    image->fill(Qt::transparent);
    //image->fill(QColor(Qt::green).rgb());

    QPainter* painter = new QPainter(image);

    painter->setRenderHint(QPainter::Antialiasing, true);       //http://qt.gitorious.org/qt-labs/graphics-dojo/blobs/master/webcapture/webcapture.cpp
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->setBackground(QBrush(Qt::white));
    painter->setRenderHints(QPainter::Antialiasing);
    frame->render(painter);
    painter->end();
    if(painter)delete painter;painter=0;

    {   //crop white ends -- begin
        int l = image->width();
        int r = 0;
        int t = image->height();
        int b = 0;

        for (int y = 0; y < image->height(); ++y)
        {
            QRgb *row = (QRgb*)image->scanLine(y);
            bool rowFilled = false;
            for (int x = 0; x < image->width(); ++x)
            {
                QRgb p = row[x];
                static QRgb white = QColor(Qt::white).rgb();

                if(qGreen(p)== qGreen(white) &&
                   qRed(p)  == qRed(white)   &&
                   qBlue(p) == qBlue(white))
                {
                    //
                }
                else
                {
                    rowFilled = true;
                    r = std::max(r, x);
                    if (l > x)
                    {
                        l = x;
                        x = r; // shortcut to only search for new right bound from here
                    }
                }
            }
            if (rowFilled)
            {
                t = std::min(t, y);
                b = y;
            }
        }
        QImage image2 = image->copy(l, t, r, b);
        QImage* pimage2= new QImage(image2);
        delete image;image=0;
        image = pimage2;
    }      //crop white ends -- end

    QString sFN = qApp->applicationDirPath()+"/web2pic";
    int i=1;
    while(QFile::exists(sFN+QString::number(i)+".png"))
    {
        i++;
    }
    if(!image->save(sFN+QString::number(i)+".png"))
    {
        helpers::log("!img.save !ok "+sFN+QString::number(i)+".png", LOG_ERR, qApp, 0);
        delete image;image=0;
        return QString::null;
    }
    helpers::log("img.save "+sFN+QString::number(i)+".png", LOG_INF, qApp, 0);
    delete image;image=0;
    return sFN+QString::number(i)+".png";
}
