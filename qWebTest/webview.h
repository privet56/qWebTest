#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>

class WebView : public QWebView
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);

    virtual QWebView* createWindow ( QWebPage::WebWindowType type);

    QString web2pic();

signals:

public slots:

};

#endif // WEBVIEW_H
