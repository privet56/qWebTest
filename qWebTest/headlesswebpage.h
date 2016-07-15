#ifndef HEADLESSWEBPAGE_H
#define HEADLESSWEBPAGE_H

#include <QTimer>
#include "webpage.h"
#include "urlinfo.h"

class HeadlessWebPage : public WebPage
{
    Q_OBJECT
public:
    explicit HeadlessWebPage(int iIndex, QObject *parent = 0);

    QString getCurrentUrl2Load();

    virtual void onHttpResponseFinished(QNetworkReply* reply);

    void load(QStringList urls);

    bool m_bLoadFinished;
    int m_iIndex;

    bool m_bCheckLoadFinished;

    QStringList m_urls;
    int m_urls_done_index;

    QList<UrlInfo*> m_urlsWithError;

    QTimer timer;

    void onLoadFinished(bool bOK, bool bTimeOut);

signals:
    void testFinished(HeadlessWebPage* hwp);

public slots:
    void onLoadFinished(bool bOK);
    void onLoadStarted();
    void onTimeOut();
    void nextTest();
    void setEmpty();
};

#endif // HEADLESSWEBPAGE_H
