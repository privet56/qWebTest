#include "doqter.h"
#include <io.h>
#ifdef Q_WS_WIN
#include "windows.h"
#include "stdlib.h"
#include "shlobj.h" //SHGetSpecialFolderPath
#endif

#include <QSslSocket>
#include "urldownloaderwp.h"
#include "ssh.h"
#include "mainwindow.h"

class InteractiveProcess : public QProcess
{
    static int stdinClone;
public:
    InteractiveProcess(QObject *parent = 0)
        : QProcess(parent)
    {
        if (stdinClone == -1)
            stdinClone = ::dup(fileno(stdin));
    }
protected:
    void setupChildProcess()
    {
        ::dup2(stdinClone, fileno(stdin));
    }
};

int InteractiveProcess::stdinClone = -1;

doqter::doqter(QObject *parent) : QObject(parent)
{
    m_pVB = m_pSSH = 0;
    m_pssh = 0;
    m_iSSHCheck = 0;
}

doqter::~doqter()
{
    if( m_pSSH) m_pSSH->kill();
    if( m_pVB)  m_pVB->kill();
    if(m_pssh)  m_pssh->kill();

    m_pVB = m_pSSH = 0;
    m_pssh = 0;
}

void doqter::doqte()
{
    QString sRootDirFrom = qApp->applicationDirPath()+QDir::separator()+"doqter_tmpl";
    QString sRootDirTo   = qApp->applicationDirPath()+QDir::separator()+"doqter";

    QTime myTimer;
    myTimer.start();
    int iWrittenFiles = tmpl(sRootDirFrom, sRootDirTo, "");
    int nMilliseconds = myTimer.elapsed();
    helpers::log("doqter:tmpl: finished "+QString::number(iWrittenFiles)+" files in "+QString::number(nMilliseconds)+" ms",LOG_INF, qApp, this->parent());

    {
        QSettings excelSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Oracle\\VirtualBox", QSettings::NativeFormat);
        QString vBoxLocation = excelSettings.value("InstallDir").toString();
        if(helpers::str_isempty(vBoxLocation, true))
        {
            QString s = "Doqter version 0.1 doesn't support systems without VirtualBox.\nVirtualBox installation not found.\n(install from ./doqter/v/vinstall/VirtualBox-4.3.26-r98988-MultiArch_x86.msi)";
            helpers::log(s,LOG_ERR, qApp, this->parent());
            QMessageBox::warning(0, "VirtualBox installation not found.", s);
            return;
        }
    }

    prep_webapp(sRootDirTo);

    bool ok = execvb(sRootDirTo);
    if(ok)
    {
        m_iSSHCheck = 0;
        //[31mFATA[0m[0000] Post http:///var/run/docker.sock/v1.18/containers/create: dial unix /var/run/docker.sock: no such file or directory. Are you trying to connect to a TLS-enabled daemon without TLS?
        m_pssh = new ssh("127.0.0.1", "docker", "tcuser", 2022, "docker run -d -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx\n", this);
        //m_pssh = new ssh("127.0.0.1", "docker", "tcuser", 2022, "docker run -it --rm -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx\n", this);
        //TODO: use shorter time frame and look for "Are you trying to connect to a TLS-enabled daemon without TLS?" in the ssh output
        QTimer::singleShot(15000, this, SLOT(execssh()));
    }
}
void doqter::prep_ssh(QString sDoqterDir)
{
    QTime myTimer;
    myTimer.start();

    QString sFrom   = helpers::makeAbsFN(sDoqterDir, ".ssh");
    QString sTo     = helpers::makeAbsFN(this->getProfileDir(), ".ssh");
    int iCopiedOrChecked = helpers::copydirifdifferent(sFrom, sTo, qApp, 0);
    int nMilliseconds = myTimer.elapsed();
    helpers::log("doqter:ssh: finished "+QString::number(iCopiedOrChecked)+" files in "+QString::number(nMilliseconds)+" ms",LOG_INF, qApp, 0);
}
void doqter::prep_webapp(QString sDoqterDir)
{
    QTime myTimer;
    myTimer.start();

    QString sFrom   = helpers::makeAbsFN(sDoqterDir, "webapp");
    QString sTo     = helpers::makeAbsFN(this->getProfileDir(), "webapp");
    {
        QDir dstsubdir(sTo);
        if(!dstsubdir.exists())
        {
            QDir dst(this->getProfileDir());
            dst.mkdir("webapp");
        }
    }
    int iCopiedOrChecked = helpers::copydirifdifferent(sFrom, sTo, qApp, 0);
    int nMilliseconds = myTimer.elapsed();
    helpers::log("doqter:prep_webapp: finished "+QString::number(iCopiedOrChecked)+" files in "+QString::number(nMilliseconds)+" ms",LOG_INF, qApp, 0);
}
void doqter::execssh()
{
    {
        if(m_iSSHCheck > 19)
        {
            helpers::log("doqter:exec: SSH not found!",LOG_ERR, qApp, this->parent());
            return;
        }

        if(m_pssh->dossh())
        {
            return;
        }

        QTimer::singleShot(3000, this, SLOT(execssh()));
        m_iSSHCheck++;
        helpers::log("doqter:exec: searching for SSH ...",LOG_INF, qApp, this->parent());
        return;
    }

    QString sDoqterDir = qApp->applicationDirPath()+QDir::separator()+"doqter";
    sDoqterDir.replace("/", "\\");

    /*{
        m_pVB->write("tcuser\n");
        m_pVB->write("docker run -it --rm -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx\n");
        helpers::log("doqter:exec: docker'd",LOG_INF, qApp, 0);
        return;
    }* /
    {
        system(QString(sDoqterDir+"\\g\\ssh2docker.bat").toStdString().c_str());
        return;
    }*/

    m_pSSH = new QProcess(this);
    QProcess* p = m_pSSH;
    p->setWorkingDirectory(sDoqterDir+"\\g");
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("PATH") + ";"+sDoqterDir+";"+sDoqterDir+"\\g");
    env.insert("VBM", sDoqterDir+"\\v");
    env.insert("VBOX_INSTALL_PATH", sDoqterDir+"\\v");
    p->setProcessEnvironment(env);
    QStringList arguments;

    if(!QFile::exists(this->getProfileDir()+"\\.ssh\\id_boot2docker"))
        helpers::log("doqter:exec: fnf: "+this->getProfileDir()+"\\.ssh\\id_boot2docker",LOG_ERR, qApp, 0);

    arguments //<< "ssh"
              << "-o" << "IdentitiesOnly=yes"
              << "-o" << "StrictHostKeyChecking=no"
              << "-o" << "UserKnownHostsFile=/dev/null"
              << "-p" << "2022"
              << "-i" << this->getProfileDir()+"\\.ssh\\id_boot2docker"
              << "docker@127.0.0.1"
              << "docker run -it --rm -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx";
              //<< "-o" << "LogLevel=quiet"                           // suppress "Warning: Permanently added '[localhost]:2022' (ECDSA) to the list of known hosts."

    p->setProcessChannelMode(QProcess::MergedChannels);  //it will cause printing everything (even stderr output) to stdout output
    //p->setInputChannelMode(QProcess::ForwardedInputChannel);
    connect(p,SIGNAL(readyRead()), this, SLOT(onReadyReadStdOutput()));
    connect(p,SIGNAL(started()), this, SLOT(onSSHStarted()));
    connect(p,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    p->setObjectName("ssh.exe");
    p->setObjectName("ssh2docker.bat");

    //ssh does not like execute!    ////time="2015-06-03T14:23:01Z" level=fatal msg="cannot enable tty mode on non tty input"
    p->start(sDoqterDir+"\\g\\"+p->objectName(), arguments);
    helpers::log("doqter:exec: "+p->objectName()+" started",LOG_INF, qApp, 0);

    //     env << "SSH_ASKPASS=" << <path to the temporary script file>;        //#!/bin/sh \n echo "<your password>"
}

void doqter::onSSHStarted()
{
    QProcess* p = (QProcess*)sender();
    p->write(QString("docker run -it --rm -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx\n").toStdString().c_str());
    helpers::log("doqter:exec: ssh -> docker run",LOG_INF, qApp, 0);
}

bool doqter::execvb(QString sDoqterDir)
{
    m_pVB = new QProcess(this);
    QProcess* p = m_pVB;
#ifdef START_BOOT2DOCKER
    p->setWorkingDirectory(sDoqterDir);
#else
    p->setWorkingDirectory(sDoqterDir+"\\v");
#endif
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    system(QString("set PATH=%PATH%;"+sDoqterDir+";"+sDoqterDir+"\\g").toStdString().c_str());
    system(QString("set VBM="+sDoqterDir+"\\v").toStdString().c_str());
    system(QString("set VBOX_INSTALL_PATH="+sDoqterDir+"\\v").toStdString().c_str());

    env.insert("PATH", env.value("PATH") + ";"+sDoqterDir+";"+sDoqterDir+"\\g");
    env.insert("VBM", sDoqterDir+"\\v");
    env.insert("VBOX_INSTALL_PATH", sDoqterDir+"\\v");
    p->setProcessEnvironment(env);
    QStringList arguments;
#ifdef START_BOOT2DOCKER
    QString sVBM = QString(sDoqterDir+"\\v\\vboxmanage.exe").replace('\\','/');
    if(!QFile::exists(sVBM))
        helpers::log("doqter:execvb: fnf:"+sVBM, LOG_ERR, qApp, this->parent());
    QString sSSH = QString(sDoqterDir+"\\g\\ssh.exe").replace('\\','/');
    if(!QFile::exists(sSSH))
        helpers::log("doqter:execvb: fnf:"+sSSH, LOG_ERR, qApp, this->parent());

    arguments << "-v" << "--vbm="+sVBM << "--ssh="+sSSH << "start" << "docker run -d -p 8080:80 -v /c/Users/"+getProfileName()+"/webapp:/usr/share/nginx/html:ro nginx";       //boot2docker.exe
#else
    arguments << "boot2docker-vm";    //p.exe
#endif
    p->setProcessChannelMode(QProcess::MergedChannels);  //it will cause printing everything (even stderr output) to stdout output
    connect(p,SIGNAL(readyRead()), this, SLOT(onReadyReadStdOutput()));
    connect(p,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
#ifdef START_BOOT2DOCKER
    p->setObjectName("boot2docker.exe");
#else
    p->setObjectName("v\\p.exe");
#endif
    //p->setObjectName("_.bat");
    int iRe = p->execute(sDoqterDir+"\\"+p->objectName(), arguments);
    helpers::log("doqter:exec: '"+p->objectName()+"' "+(iRe==0 ? "" : "NOT")+" started. return code: "+QString::number(iRe),iRe == 0 ? LOG_INF : LOG_ERR, qApp, this->parent());
    return iRe == 0 ? true : false;
}
QString doqter::getProfileName()
{
    QString sProfileDir = this->getProfileDir().replace("/", "\\");
    QStringList asProfileDir = sProfileDir.split("\\");
    return asProfileDir.at(asProfileDir.size() - 1);
}
QString doqter::getProfileDir()
{
    static QString home = QString::null;
    static bool bHome = false;
    if(bHome)
        return home;
    bHome = true;

#ifdef Q_WS_WIN
    TCHAR lpBuffer[_MAX_PATH];
    SHGetSpecialFolderPath(NULL, lpBuffer, CSIDL_PROFILE, 0);   //CSIDL_* list -> see: http://msdn.microsoft.com/en-us/library/ms954149.aspx
    home = QString::fromUtf16(lpBuffer);
    home.replace("/", "\\");
#else
    home = QDir::homePath();
#endif
    return home;
}

void doqter::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    QProcess* p = (QProcess*)sender();
    helpers::log("doqter:exec: '"+p->objectName()+"' exited: "+QString::number(exitCode),LOG_WRN, qApp, 0);
}

void doqter::onReadyReadStdOutput()
{
    QProcess* p = (QProcess*)sender();
    QString s = p->readAll();
    s = s.trimmed();
    helpers::log("doqter:readstdout: '"+p->objectName()+"' STDOUT\n------------------- "+s+"\n------------------------",LOG_INF, qApp, 0);
}

int doqter::tmpl(QString sRootDirFrom, QString sRootDirTo, QString sSubDir)
{
    int iRe = 0;
    QString sFrom = helpers::makeAbsFN(sRootDirFrom, sSubDir);
    QString sTo   = helpers::makeAbsFN(sRootDirTo  , sSubDir);
    QDir src(sFrom);
    QDir dst(sTo);

    QFileInfoList contents = src.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo fileInfo, contents)
    {
        QString fileName = fileInfo.fileName();
        QString srcFilePath = src.absoluteFilePath(fileName);
        QString dstFilePath = dst.absoluteFilePath(fileName);

        if (fileInfo.isDir())
        {
            QDir dstsubdir(dstFilePath);
            if(!dstsubdir.exists() && !dst.mkdir(fileName))
            {
                helpers::log("doqter:tmpl: !dnf "+dstFilePath,LOG_ERR, qApp, 0);
            }
            iRe += tmpl(sRootDirFrom, sRootDirTo, helpers::makeAbsFN(sSubDir, fileName));
        }
        else if (fileInfo.isFile())
        {
            QString sFC = helpers::GetFileContent(srcFilePath, true, qApp, 0);
            static QString CURRENTDIR               = qApp->applicationDirPath().replace("/", "\\");
            static QString CURRENTDIR2BACKSLASHES   = CURRENTDIR;
            static QString TMPDIR                   = QDir::tempPath().replace("/", "\\");
            static QString TMPDIR2BACKSLASHES       = TMPDIR;
            static QString PROFILEDIR               = this->getProfileDir();

            static bool bFirst = true;
            if(bFirst)
            {
                CURRENTDIR2BACKSLASHES  = CURRENTDIR2BACKSLASHES.replace("\\", "\\\\");
                TMPDIR2BACKSLASHES      = TMPDIR2BACKSLASHES.replace("\\", "\\\\");
                bFirst = false;
/*
helpers::log("doqter:tmpl: CURRENTDIR:'"+CURRENTDIR+"'",LOG_INF, qApp, 0);
helpers::log("doqter:tmpl: CURRENTDIR2BACKSLASHES:'"+CURRENTDIR2BACKSLASHES+"'",LOG_INF, qApp, 0);
helpers::log("doqter:tmpl: TMPDIR:'"+TMPDIR+"'",LOG_INF, qApp, 0);
helpers::log("doqter:tmpl: TMPDIR2BACKSLASHES:'"+TMPDIR2BACKSLASHES+"'",LOG_INF, qApp, 0);
*/
            }

            sFC.replace("%CURRENT\\\\DIR%", CURRENTDIR2BACKSLASHES);
            sFC.replace("%TMP\\\\DIR%", TMPDIR2BACKSLASHES);
            sFC.replace("%CURRENTDIR%", CURRENTDIR);
            sFC.replace("%TMPDIR%", TMPDIR);
            sFC.replace("%PROFILEDIR%", PROFILEDIR);

            helpers::WriteFileContent(dstFilePath, sFC, true, qApp, 0);

            iRe++;

//helpers::log("doqter:tmpl: written:'"+dstFilePath+"'",LOG_INF, qApp, 0);
        }
        else
        {
            helpers::log("doqter:tmpl: !?dnf? "+srcFilePath,LOG_ERR, qApp, 0);
        }
    }

    return iRe;
}
void doqter::log(QString s, int iLevel, QApplication* pApp, bool bWithDate)
{
    MainWindow* pmw = (MainWindow*)this->parent();

    pmw->log(s, iLevel, pApp, bWithDate);

    if(s.indexOf(CALLSSHCOMMAND) > 1)
    {
        QTimer::singleShot(3000, this, SLOT(execload()));
    }
}
void doqter::execload()
{
    MainWindow* pmw = (MainWindow*)this->parent();
    pmw->load("http://127.0.0.1:1234/");
}
