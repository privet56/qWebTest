#include "mainwindow.h"
#include "networkaccessmanager.h"
#include "headlessloadtest.h"
#include "headlesswebpagelogger.h"
#include "ui_mainwindow.h"
#include "printwebpage.h"
#include "printwebview.h"
#include <QPrinter>
#include <QTimer>
#include <QWebFrame>
#include <QWebSecurityOrigin>
#include <QMessageBox>
#include "CollectMailsMongo.h"
#include "collectmailsesb.h"
#include "urldownloader.h"
#include "urldownloaderwp.h"
#include "doqter.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QCoreApplication::setOrganizationName("SC");
    QCoreApplication::setOrganizationDomain("SC");
    QCoreApplication::setApplicationName("webtest");
    QCoreApplication::setApplicationVersion("1.0.0");
    //QSettings settings(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    ui->setupUi(this);
    this->setWindowTitle(" ");

    //circle!
    //QRegion* region = new QRegion(*(new QRect(x()+5,y()+5,370,370)),QRegion::Ellipse);
    //setMask(*region);
    //this->setAttribute(Qt::WA_TranslucentBackground, true); //-> black (overrides css)
    //this->setRoundedCorners(0, 0, 9, 9);

    //this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    //this->setWindowFlags(Qt::FramelessWindowHint);

    networkAccessManager* pNetworkManager = new networkAccessManager(this->ui->webView->page()->networkAccessManager(), this->ui->webView);
    this->ui->webView->page()->setNetworkAccessManager(pNetworkManager);

    {
        QString sUrl = helpers::GetCFG("url", qApp, 0).trimmed();
        if(!helpers::str_isempty(sUrl, true))
        {
            this->ui->lineEdit->setText(sUrl);
            //WebPage::beforeLoadUrl(this->ui->webView->page(), sUrl);
            QUrl u(sUrl, QUrl::StrictMode);

            if(!helpers::str_isempty(helpers::GetCFG("un", qApp, 0), true)) //QMessageBox::warning(this, "un+pwd", "un:'"+u.userName()+"'\npwd:'"+u.password()+"'");
            {
                u.setUserName(helpers::GetCFG("un" , qApp, 0));
                u.setPassword(helpers::GetCFG("pwd", qApp, 0));
            }
            if(sUrl.startsWith("file://"))
                this->ui->webView->setUrl(this->ui->lineEdit->text());
            else
                this->ui->webView->load(u);
        }
    }

    this->ui->pushButton->setani(":/res/res/record.gif");
    this->ui->pushButton->setEnabled(ScreenShooter::canScreenShoot());

    QTimer::singleShot(1000, this, SLOT(oninit()));
}

void MainWindow::setRoundedCorners(int radius_tl, int radius_tr, int radius_bl, int radius_br)
{
    QRegion region(-26, -26, width()+23, height()+23, QRegion::Rectangle);

    // top left
    QRegion round (0, 0, 2*radius_tl, 2*radius_tl, QRegion::Ellipse);
    QRegion corner(0, 0, radius_tl, radius_tl, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));

    // top right
    round = QRegion(width()-2*radius_tr, 0, 2*radius_tr, 2*radius_tr, QRegion::Ellipse);
    corner = QRegion(width()-radius_tr, 0, radius_tr, radius_tr, QRegion::Rectangle);
//    region = region.subtracted(corner.subtracted(round));

    // bottom right
    round = QRegion(width()-2*radius_br, height()-2*radius_br, 2*radius_br, 2*radius_br, QRegion::Ellipse);
    corner = QRegion(width()-radius_br, height()-radius_br, radius_br, radius_br, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));

    // bottom left
    round = QRegion(0, height()-2*radius_bl, 2*radius_bl, 2*radius_bl, QRegion::Ellipse);
    corner = QRegion(0, height()-radius_bl, radius_bl, radius_bl, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));

    setMask(region);
}

void MainWindow::oninit()
{
    QString sMode = helpers::GetCFG("mode", qApp, 0).toLower().trimmed();
    if(sMode == "codetest")
    {
        //ssh* _ssh = new ssh(0);
        //_ssh->dossh();

        //collectmailsesb::__testjson();
        /*for(int i=0;i<6;i++)
        {
            QString sUrl = "https://outlook.office365.com/api/v1.0/me/folders/";
            //QString sUrl = "https://www.youtube.com/";
            QString s = urldownloaderwp::load(this, sUrl, "name%40email.com", "pwd");
            helpers::log("oninit i:"+QString::number(i)+" re-len:"+QString::number(s.size())+"\n-------------"+s+"\n-------------", s.size() > 1 ? LOG_INF : LOG_ERR, qApp, 0);
        }*/
        helpers::log("oninit "+sMode+" FINISH", LOG_INF, qApp, 0);
    }
    if(sMode == "mongomails")
    {
        CollectMailsMongo* cmm = new CollectMailsMongo(this, this->ui->webView->page()->networkAccessManager());
        cmm->collect();
    }
    if(sMode == "esbmails")
    {
        collectmailsesb* cmm = new collectmailsesb(this);
        cmm->collect();
    }
    else if(sMode == "loadtest")
    {
        HeadlessLoadTest* hlt = new HeadlessLoadTest();
        hlt->loadtest();
    }
    else if(sMode == "webpagelogger")
    {
        HeadlessWebPageLogger* wpl = new HeadlessWebPageLogger();
        wpl->load("http://www.heise.de/");
    }
    else if((sMode == "docker") || (sMode == "doqter"))
    {
        doqter* dqtr = new doqter(this);
        dqtr->doqte();
    }
    else if(sMode == "web2pic")
    {
        return;     //handled in void MainWindow::on_webView_loadFinished(bool bOK)
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    WebPage::beforeLoadUrl(this->ui->webView->page(), this->ui->lineEdit->text());
    this->ui->webView->setUrl(this->ui->lineEdit->text());
}

void MainWindow::on_webView_objectNameChanged(const QString &objectName)
{
    Q_UNUSED(objectName);
}

void MainWindow::on_webView_iconChanged()
{
    QIcon icon = QWebSettings::iconForUrl(this->ui->webView->url());
    if(!icon.isNull())
    {
        QWidget* p = this->parentWidget();
        while(p)
        {
            p->setWindowIcon(icon);
            p = p->parentWidget();
        }
    }
}

void MainWindow::on_webView_linkClicked(const QUrl &arg1)
{
    Q_UNUSED(arg1);
}

void MainWindow::on_webView_loadFinished(bool bOK)
{
    if(!bOK)
    {
        on_webView_titleChanged("error loading "+this->ui->webView->url().toString());
        helpers::log("load !ok "+this->ui->webView->url().toString(), LOG_WRN, qApp, 0);
    }

    QString sMode = helpers::GetCFG("mode", qApp, 0).toLower().trimmed();

    if(sMode == "web2pic")
    {
        this->ui->webView->web2pic();
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::on_webView_loadStarted()
{

}

void MainWindow::on_webView_titleChanged(const QString &title)
{
    this->setWindowTitle("Qt WebRecorder & Automation - "+title);
}

void MainWindow::on_webView_urlChanged(const QUrl &arg1)
{
    Q_UNUSED(arg1);
}

void MainWindow::on_webView_windowIconChanged(const QIcon &icon)
{
    this->setWindowIcon(icon);
}

void MainWindow::on_webView_windowTitleChanged(const QString &title)
{
    this->setWindowTitle(title);
}

void MainWindow::on_webView_windowIconTextChanged(const QString &iconText)
{
    Q_UNUSED(iconText);
}

void MainWindow::on_pushButton_toggled(bool checked)
{
    if(checked)
    {
        m_screenshooter = new ScreenShooter();
        m_screenshooter->start(this);
    }
    else
    {
        m_screenshooter->createani();
        delete m_screenshooter;
        m_screenshooter = 0;
    }
}

void MainWindow::load(QString sUrl)
{
    this->ui->lineEdit->setText(sUrl);
    this->ui->webView->setUrl(this->ui->lineEdit->text());
}

void MainWindow::log(QString s, int iLevel, QApplication* pApp, bool bWithDate)
{
    Q_UNUSED(pApp);

    s = s.replace('\'',' ');
    QString sjs = QString("log('"+s+"', "+QString::number(iLevel)+", "+QString::number(bWithDate)+");");
    QVariant v = this->ui->webView->page()->mainFrame()->evaluateJavaScript(sjs);
    QString sjsre = v.value<QString>();
}
