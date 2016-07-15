#ifndef URLDOWNLOADER_H
#define URLDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QHttpPart>
#include <QUrl>
#include <QSslConfiguration>

class urldownloader : public QObject
{
    Q_OBJECT
public:
    explicit urldownloader(QObject *parent, QNetworkAccessManager* pMan, QString url, QString un, QString pwd);
    QNetworkAccessManager* m_pMan;
    QNetworkReply* m_pNResp;
    QByteArray m_baResp;
    QEventLoop m_loop;
    QUrl m_url;

    int m_iErrorCode;

    static QString load(QObject *parent, QNetworkAccessManager* pMan, QString url, QString un, QString pwd);

signals:

public slots:
    void slot_downloadReady();
    void sslErrors(QNetworkReply* reply, const QList<QSslError> & errors);
    void error(QNetworkReply::NetworkError code);
    void sslErrors(const QList<QSslError> & errors);
};

#endif // URLDOWNLOADER_H
