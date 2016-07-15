#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

class WebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit WebPage(bool bSilent, QObject *parent = 0);

    virtual bool	acceptNavigationRequest ( QWebFrame * frame, const QNetworkRequest & request, NavigationType type);
    virtual void	javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID );
    virtual void    onHttpResponseFinished(QNetworkReply* reply);

    static void beforeLoadUrl(QWebPage* page, QString sUrl);

signals:

public slots:
    void httpResponseFinished(QNetworkReply* reply);
};

#endif // WEBPAGE_H
