#ifndef DOQTER_H
#define DOQTER_H

#include <QObject>
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

#include "helpers.h"
#include "ssh.h"

//#define START_BOOT2DOCKER

class doqter : public QObject
{
    Q_OBJECT
public:
    explicit doqter(QObject *parent = 0);
    ~doqter();

    QProcess* m_pVB;
    QProcess* m_pSSH;
    ssh* m_pssh;
    int m_iSSHCheck;

    void doqte();
    int tmpl(QString sRootDirFrom, QString sRootDirTo, QString sSubDir);
    bool execvb(QString sDoqterDir);
    void prep_ssh(QString sDoqterDir);
    void prep_webapp(QString sDoqterDir);
    QString getProfileDir();
    QString getProfileName();

signals:

public slots:
    void onReadyReadStdOutput();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void execssh();
    void execload();
    void onSSHStarted();
    void log(QString s, int iLevel, QApplication* pApp, bool bWithDate);
};

#endif // DOQTER_H
