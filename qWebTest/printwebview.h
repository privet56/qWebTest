#ifndef PRINTWEBVIEW_H
#define PRINTWEBVIEW_H

#include <QObject>
#include "webview.h"

class PrintWebView : public WebView
{
    Q_OBJECT
public:
    explicit PrintWebView(QWidget *parent = 0);

    void print(QUrl url);

signals:

public slots:
    void onLoadFinished(bool bOK);
};

#endif // PRINTWEBVIEW_H
