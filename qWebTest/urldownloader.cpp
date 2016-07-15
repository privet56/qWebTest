#include "urldownloader.h"
#include <QNetworkProxyFactory>
#include "helpers.h"

urldownloader::urldownloader(QObject *parent, QNetworkAccessManager* pMan, QString url, QString un, QString pwd) : QObject(parent)
{
    m_iErrorCode = 0;
    this->m_pMan = pMan;

    QNetworkRequest* pNReq = new QNetworkRequest();
    QUrl u(url, QUrl::StrictMode);
    if(!helpers::str_isempty(un, true)) u.setUserName(un);
    if(!helpers::str_isempty(pwd,true)) u.setPassword(pwd);
    pNReq->setUrl(u);

    pNReq->setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    pNReq->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:33.0) Gecko/20100101 Firefox/33.0");
    pNReq->setRawHeader("Accept", "text/html,application/xhtml+xml,application/json,application/xml;q=0.9,*/*;q=0.8");
    pNReq->setRawHeader("Host", "outlook.office365.com");
    pNReq->setRawHeader("Accept-Language", "de,en-US;q=0.7,en;q=0.3");
    pNReq->setRawHeader("Accept-Encoding", "gzip, deflate");
    //pNReq->setRawHeader("Cookie", "X-BackEndCookie2=geeg@b.com=u56Lnp2ejJqBy57Lmc3LzMvSm56ZydLLns+Z0sfOzZ7Sm8mdm5uanc3PzszPgZGemJ6NjZDRnJCSgc3PzsrSz87Szs6rzszFzc/Fys+Bno+cj42bz8nRj42Qm9GQiouTkJCU0ZyQkg==; X-BackEndCookie=geeg@b.com=u56Lnp2ejJqBy57Lmc3LzMvSm56ZydLLns+Z0sfOzZ7Sm8mdm5uanc3PzszPgZGemJ6NjZDRnJCSgc3PzsrSz87Szs6rzszFzc/Fys8=; ClientId=S8ZGAYKKZUMUUD8Z1GAHUA; RPSAuth=FABKARQl3luxXpSMs941OuaciuqbomfY%2BwNmAAAEgAAACHn%2Bcn5qeThRCAFUFhV4cLRyTxhKnj3Cu9/UurHnSzTjUaVlDJYm%2BQYvG5gQJK/8/zoZ7Ql2l4gPN8g42lt7Q/4qvIm9kuW4Pu7BML9c%2B/%2BbrbgBQt/J0k9SuVs2N05Ttdae%2B%2Bqej/mKA/5Mwu0XHLdZxwjkYNftPQXVA%2BUR35tt6PDDH3IyQ2ETfrJQzq0wEalYIugrYegzPwUoQdnEsaswOEsadSBVLx%2Bp8C%2BRkBC/SRApp4jDexCEy4Vfpfflhqs8ACrDCsjYANA34KdLa2p8TJgYhyc01hqmUSXCK/uKi2bLI70AzhPtIPWH0G09u6hi7NmgsqtpgmwM91oZymAhHxg8zz1zu3kp7KqojW%2BlKIQUAECcHToiItuRNYis%2Be/YQk0rDOV7; RPSSecAuth=FABKARQl3luxXpSMs941OuaciuqbomfY%2BwNmAAAEgAAACFayulSMa1YxCAF20P9w0quS9gl%2BL2ogHL40UIhPjFT7rNpGHzpI1HDUzb7vzElMdFwxn9bNh0b9pmxshVO1cnThVnpSe620285EW7j5cPqQtRhrkh4B9pfns1rGRbCVVncizhPaa0TNnzV98T7qSKVki0JbAIjmo3f8V0SNw6prVwnugAVThyu9zpM6NmTX5azYBihvq5wSmWxEDLXvBFcQ4TPcDfNKLbI3dC243KYjJDfS%2BSEbrc819OCDjIs8BsVttnhNU1y2AKh9p3sSg2t3f8T5nYWdBQajS5qR7WiZkJNJAqUfoYWe9CMMt0QOkHC8axIj140uohggNVZxtEhSeZTBBNqxGM4aWn8cIR0x11AUAPWwpDhKFCQ/8KFVxTfEVDI6xy73; MH=MSFT; exchangecookie=ac8b9135b4d748f0854d00baf15f5175");
    //pNReq->setRawHeader("Authorization", "Basic Z2VvcmdlLmhlcmN6ZWdAbmFnYXJyby5jb206XzFTb2Z0Y29u");

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    {
        QSslConfiguration cfg = pNReq->sslConfiguration();
        cfg.setProtocol(QSsl::UnknownProtocol);
        pNReq->setSslConfiguration(cfg);
    }
    m_url = u;
    m_pNResp = pMan->get(*pNReq);
    connect(m_pNResp, SIGNAL(finished()), &m_loop, SLOT(quit()));
    connect(m_pNResp, SIGNAL(readyRead()), this, SLOT(slot_downloadReady()));
    connect(m_pNResp, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect(m_pNResp, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(sslErrors(const QList<QSslError>&)));

    m_loop.exec();
}
void urldownloader::slot_downloadReady()
{
    m_baResp.append(m_pNResp->readAll());
}
void urldownloader::sslErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
    Q_UNUSED(reply)
helpers::log("urld:sslErrors(1)", LOG_WRN, qApp, 0);
    for(int i=0; i < errors.length(); i++)
    {
        QSslError e = errors.at(i);
        //__log(e.errorString());
    }
}
void urldownloader::error(QNetworkReply::NetworkError code)
{
    //5     = QNetworkReply::OperationCanceledError
    //6     = QNetworkReply::SslHandshakeFailedError
    //204   = No Content
    //299   = QNetworkReply::UnknownContentError
    m_iErrorCode = code;
helpers::log("urld:error code:"+QString::number(code)+" url:'"+this->m_url.toString()+"'\n\tun::'"+this->m_url.userName()+"'\n\tpwd:'"+this->m_url.password()+"'", LOG_WRN, qApp, 0);
}
void urldownloader::sslErrors(const QList<QSslError> & errors)
{
    Q_UNUSED(errors)
    helpers::log("urld:sslErrors(2)", LOG_WRN, qApp, 0);
}

QString urldownloader::load(QObject *parent, QNetworkAccessManager* pMan, QString url, QString un, QString pwd)
{
    for(int i=0;i<9;i++)
    {
        urldownloader* pud = new urldownloader(parent, pMan, url, un, pwd);
        QString s = pud->m_baResp;
        if((pud->m_iErrorCode == 204) || helpers::str_isempty(s, false))
        {
            helpers::log("urld:load("+QString::number(i)+"): "+QString::number(pud->m_iErrorCode), LOG_WRN, qApp, 0);
            if((i % 2) == 0)helpers::qWait(3);
            continue;
        }
        if(i > 0)
        {
            helpers::log("urld:load OK("+QString::number(i)+"): "+QString::number(pud->m_iErrorCode), LOG_INF, qApp, 0);
        }
        return s;
    }
    return "";
}
