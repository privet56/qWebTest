#include "ssh.h"

ssh::ssh(QString sUrl, QString un, QString pw, int iPort, QString sCmd, QObject *parent) : QObject(parent)
{
#ifdef USE_QSSH
    params.host     = sUrl;
    params.userName = un;
    params.password = pw;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationByPassword;
    params.timeout  = 30;
    params.port     = iPort;

    m_shell.clear();
    m_connection = 0;
#else
    socket = 0;
#endif
    m_sCmd = sCmd;
}

ssh::~ssh()
{
    kill();
}

void ssh::kill()
{
    if(!m_shell.isNull())m_shell->close();
    if(m_connection)m_connection->closeAllChannels();
    m_shell.clear();
    m_connection = 0;
}

bool ssh::dossh()
{
#ifdef USE_QSSH
    {
        if(m_connection && m_connection->state() != QSsh::SshConnection::Unconnected)
        {
            helpers::log("ssh: already connecting...", LOG_INF, qApp, 0);
            return true;
        }

        m_connection = new QSsh::SshConnection(params, this);
        connect(m_connection, SIGNAL(connected()), SLOT(onQsshConnected()));
        connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onQsshConnectionError(QSsh::SshError)));
        helpers::log("ssh: connecting START...", LOG_INF, qApp, 0);
        m_connection->connectToHost();
        return false;
    }
#else
    helpers::log("ssh: START: " + QString::number(QSslSocket::supportsSsl()), QSslSocket::supportsSsl() ? LOG_INF : LOG_ERR, qApp, 0);

//http://stackoverflow.com/questions/15213139/simple-qssl-client-server-cannot-start-handshake-on-non-plain-connection

    QSslSocket *socket = new QSslSocket(this);

    socket->ignoreSslErrors();
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    socket->setProtocol(QSsl::SslV3);

    connect(socket, SIGNAL(encrypted()), this, SLOT(ready()));
    connect(socket, SIGNAL(encryptedBytesWritten(qint64)), this, SLOT(encryptedBytesWritten(qint64)));
    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)), this, SLOT(modeChanged(QSslSocket::SslMode)));
    connect(socket, SIGNAL(peerVerifyError(const QSslError &)), this, SLOT(peerVerifyError(const QSslError &)));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErrors(const QList<QSslError> &)));

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(hostFound()), this, SLOT(hostFound()));
    connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    {
        {
              QFile file( "c:/Users/gherczeg/.ssh/id_boot2docker" );
              if( ! file.open( QIODevice::ReadOnly ) )
              {
                  QMessageBox::question(0, "Erreur", "Impossible de charger id_boot2docker");
                  return;
              }
              QSslKey key(&file);
              file.close();
              helpers::log("ssh:keyok: "+QString::number(!key.isNull()), !key.isNull() ? LOG_INF : LOG_ERR, qApp, 0);
              socket->setPrivateKey( key );
        }
        foreach (const QSslCertificate &cert, QSslCertificate::fromPath("c:/Users/gherczeg/.boot2docker/certs/boot2docker-vm/*.pem", QSsl::Pem, QRegExp::Wildcard))
        {
            helpers::log("ssh:certok1: "+QString::number(!cert.isNull()), !cert.isNull() ? LOG_INF : LOG_ERR, qApp, 0);
            socket->setLocalCertificate( cert );
            socket->sslConfiguration().caCertificates().append(cert);
            socket->addCaCertificate( cert );
            socket->addDefaultCaCertificate(cert);
        }
    }

    socket->connectToHostEncrypted("127.0.0.1", 2022);
    //socket->connectToHost("127.0.0.1", 2022);

    bool bok = socket->waitForEncrypted(100000);
    //bool bok = socket->waitForConnected(100000);
    if(!bok)
    {
        helpers::log("ssh:!waited:"+QString::number(bok),LOG_ERR, qApp, 0);
        return;
    }
    helpers::log("ssh:waited4ecnrypt/connect:"+QString::number(bok),LOG_INF, qApp, 0);
    socket->startClientEncryption();
    bool wait4Read1 = socket->waitForReadyRead(100000);
    helpers::log("ssh:wait4Read1:"+QString::number(wait4Read1),wait4Read1 ? LOG_INF : LOG_ERR, qApp, 0);
    QString s = "docker: do!";
    qint64 written = socket->write(s.toStdString().c_str());
    helpers::log("ssh:written:"+QString::number(written),written > 0 ? LOG_INF : LOG_ERR, qApp, 0);
    bool flushed = socket->flush();
    helpers::log("ssh:flush:"+QString::number(flushed),flushed ? LOG_INF : LOG_ERR, qApp, 0);
    bool wait4Write = socket->waitForBytesWritten(100000);
    helpers::log("ssh:wait4Write:"+QString::number(wait4Write),wait4Write ? LOG_INF : LOG_ERR, qApp, 0);
    bool wait4Read2 = socket->waitForReadyRead(100000);
    helpers::log("ssh:wait4Read2:"+QString::number(wait4Read2),wait4Read2 ? LOG_INF : LOG_ERR, qApp, 0);
    socket->disconnectFromHost();
#endif
}
#ifdef USE_QSSH
void ssh::onQsshConnected()
{
    m_shell = m_connection->createRemoteShell();

    connect(m_shell.data(), SIGNAL(started()), SLOT(handleRemoteShellStarted()));
    connect(m_shell.data(), SIGNAL(readyReadStandardOutput()), SLOT(handleRemoteStdout()));
    connect(m_shell.data(), SIGNAL(readyReadStandardError()), SLOT(handleRemoteStderr()));
    connect(m_shell.data(), SIGNAL(closed(int)), SLOT(handleRemoteChannelClosed(int)));

    m_shell->start();
}

void ssh::handleRemoteShellStarted()
{
    helpers::log("ssh:rshell:started",LOG_INF, qApp, this->parent());
}
void ssh::handleRemoteStdout()
{
    QString sRemoteStdOut(m_shell->readAllStandardOutput().data());
    sRemoteStdOut = sRemoteStdOut.trimmed().trimmed().trimmed();
    sRemoteStdOut = sRemoteStdOut.trimmed();

    if(helpers::str_isempty(sRemoteStdOut, true))return;

    helpers::log("ssh:rshell:stdout: "+sRemoteStdOut, LOG_INF, qApp, this->parent());

    if(sRemoteStdOut.endsWith(":~$"))
    {
        if(!helpers::str_isempty(m_sCmd, true))
        {
            helpers::log("ssh:rshell:stdout: "+ QString(CALLSSHCOMMAND) + m_sCmd, LOG_INF, qApp, this->parent());
            m_shell->write(m_sCmd.toStdString().c_str());
            m_sCmd = QString::null;
        }
    }
}
void ssh::handleRemoteStderr()
{
    helpers::log("ssh:rshell:stderr: "+QString(m_shell->readAllStandardError().data()),LOG_INF, qApp, this->parent());
}
void ssh::handleRemoteChannelClosed(int i)
{
    helpers::log("ssh:rshell:close("+QString::number(i)+")",LOG_INF, qApp, this->parent());
}
void ssh::onQsshConnectionError(QSsh::SshError e)
{
    helpers::log("ssh:onQsshConnectionError: "+QString::number(e)+"  "+m_connection->errorString(),LOG_WRN, qApp, this->parent());
}
void ssh::onQsshInitialized()
{
    helpers::log("ssh:initialized",LOG_INF, qApp, 0);
}
void ssh::onQsshInitializationFailed(QString s)
{
    helpers::log("ssh:initializationFailed. "+s,LOG_ERR, qApp, this->parent());
}
#else
//SSLSOCKET SLOTS BEGIN
void ssh::ready()
{
    helpers::log("ssh:ready",LOG_INF, qApp, 0);
}
void ssh::encryptedBytesWritten(qint64 written)
{
    helpers::log("ssh:encryptedBytesWritten: "+QString::number(written),LOG_INF, qApp, 0);
}
void ssh::modeChanged(QSslSocket::SslMode mode)
{
    //1 = QSslSocket::SslClientMode
    helpers::log("ssh:modeChanged: "+QString::number(mode),LOG_INF, qApp, 0);
}
void ssh::peerVerifyError(const QSslError & error)
{
    helpers::log("ssh:peerVerifyError",LOG_INF, qApp, 0);
}
void ssh::sslErrors(const QList<QSslError> & errors)
{
    helpers::log("ssh:sslErrors",LOG_ERR, qApp, 0);
}
void ssh::connected()
{
    helpers::log("ssh:connected",LOG_INF, qApp, 0);
}
void ssh::disconnected()
{
    helpers::log("ssh:disconnected",LOG_INF, qApp, 0);
}
void ssh::error(QAbstractSocket::SocketError socketError)
{
    //13 = QAbstractSocket::SslHandshakeFailedError
    //5  = QAbstractSocket::SocketTimeoutError
    //21 = QAbstractSocket::SslInvalidUserDataError
    helpers::log("ssh:error: "+QString::number(socketError)+" "+socket->errorString(),LOG_ERR, qApp, 0);
}
void ssh::hostFound()
{
    helpers::log("ssh:hostFound",LOG_INF, qApp, 0);
}
void ssh::proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
{
    helpers::log("ssh:proxyAuthenticationRequired",LOG_INF, qApp, 0);
    authenticator->setUser("docker");
    authenticator->setPassword("tcuser");
}
void ssh::stateChanged(QAbstractSocket::SocketState socketState)
{
    helpers::log("ssh:stateChanged: "+QString::number(socketState),LOG_INF, qApp, 0);
}
void ssh::readyRead()
{
    helpers::log("ssh:readyRead----------------------\n"+socket->readAll()+"\n-------------------",LOG_INF, qApp, 0);
}
//SSLSOCKET SLOTS END
#endif
