#ifndef SSH_H
#define SSH_H

#include <QObject>
#include <QSslSocket>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QProcess>
#include <QAuthenticator>
#include <QMessageBox>
#include <QSslKey>
#include <QSslConfiguration>

#include "helpers.h"
#include "sshconnection.h"
#include "sshremoteprocess.h"

#define USE_QSSH

#define CALLSSHCOMMAND "call SSH command: "

class ssh : public QObject
{
    Q_OBJECT
public:
#ifdef USE_QSSH
    QSsh::SshConnectionParameters params;
    QSsh::SshConnection* m_connection;
    QSharedPointer<QSsh::SshRemoteProcess> m_shell;
#else
    QSslSocket *socket;
#endif
    QString m_sCmd;

    explicit ssh(QString sUrl, QString un, QString pw, int iPort, QString sCmd, QObject *parent = 0);
    ~ssh();
    bool dossh();
    void kill();

signals:

public slots:
#ifdef USE_QSSH
    void onQsshConnected();
    void onQsshConnectionError(QSsh::SshError);
    void onQsshInitialized();
    void onQsshInitializationFailed(QString);

    void handleRemoteShellStarted();
    void handleRemoteStdout();
    void handleRemoteStderr();
    void handleRemoteChannelClosed(int i);
#else
    void ready();
    void encryptedBytesWritten(qint64 written);
    void modeChanged(QSslSocket::SslMode mode);
    void peerVerifyError(const QSslError & error);
    void sslErrors(const QList<QSslError> & errors);
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void hostFound();
    void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);
    void stateChanged(QAbstractSocket::SocketState socketState);
    void readyRead();
#endif
};

#endif // SSH_H
