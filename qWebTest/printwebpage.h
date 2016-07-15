#ifndef PRINTWEBPAGE_H
#define PRINTWEBPAGE_H

#include <QObject>
#include "webpage.h"

class PrintWebPage : public WebPage
{
    Q_OBJECT
public:
    explicit PrintWebPage(QObject *parent = 0);
    void print(QUrl url);
    QString web2pic();

signals:

public slots:
    void onLoadFinished(bool bOK);
};

#endif // PRINTWEBPAGE_H
