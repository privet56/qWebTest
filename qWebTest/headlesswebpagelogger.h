#ifndef HEADLESSWEBPAGELOGGER_H
#define HEADLESSWEBPAGELOGGER_H

#include <QObject>
#include <QTimer>
#include "webpage.h"
#include "urlinfo.h"

class HeadlessWebPageLogger : public WebPage
{
    Q_OBJECT
public:
    explicit HeadlessWebPageLogger(QObject *parent = 0);

    virtual void onHttpResponseFinished(QNetworkReply* reply);

    void load(QString sUrl);

signals:

public slots:
    void onLoadFinished(bool bOK);

};

#endif // HEADLESSWEBPAGELOGGER_H
