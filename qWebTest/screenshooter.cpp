#include "screenshooter.h"
#include <QProcess>

ScreenShooter::ScreenShooter(QObject *parent) : QObject(parent)
{
    m_pTimer = 0;
    m_w2shoot = 0;
    m_iScreenshot = 0;

    static QString sDir = qApp->applicationDirPath()+QDir::separator()+"gifmovie";
    {
        QDir appDir(qApp->applicationDirPath());
        appDir.mkdir("gifmovie");
    }
}

bool ScreenShooter::canScreenShoot()
{
    static QString sExe = qApp->applicationDirPath()+QDir::separator()+"imgmagick" + QDir::separator() + "convert.exe";
    sExe = sExe.replace('/', '\\');
    return QFile::exists(sExe);
}

ScreenShooter::~ScreenShooter()
{
    stop();
}

void ScreenShooter::onMakeScreenshot()
{
    //TODO: with blue header title + with cursor
    /*{
        QObject* po = this->parent();           //both parents are 0
        QWidget* pw = this->parentWidget();
        helpers::log("pw:"+QString::number(((int)pw))+" po:"+QString::number((int)po), LOG_WRN, qApp, 0);
    }*/
    this->m_iScreenshot++;

    QString sfn = QString("%1").arg(this->m_iScreenshot, 5, 10, QChar('0'));
    static QString sDir = qApp->applicationDirPath()+QDir::separator()+"gifmovie";
    sfn = sDir + QDir::separator()  +  sfn + ".png";

    QImage bmp(m_w2shoot->size(), QImage::Format_ARGB32);
    QPainter painter(&bmp);
    m_w2shoot->render(&painter);

    //TODO: use desktop-ScreenGrabber!
    //TODO: use QtMEL screengrabber

    /*{
        QRect widgetRect = m_w2shoot->geometry();
        QPoint mousePos =  m_w2shoot->cursor().pos();
        if(widgetRect.contains(mousePos))
        {
            helpers::log("ScreenShooter:onMakeScreenshot ... mouse IS in win", LOG_INF, qApp, 0);

            QCursor cursor(::GetCursor());//m_w2shoot->cursor();
            const QBitmap* cursorBmp = cursor.bitmap();
            QPixmap cursorPxm = cursor.pixmap();

            if((cursorBmp == 0) || cursorBmp->isNull())
            {
                helpers::log("ScreenShooter:onMakeScreenshot !bmp '"+QString::number((int)cursorBmp)+"'", LOG_ERR, qApp, 0);
            }

            QImage  cursorImg = cursorPxm.toImage();
            QPoint pt = m_w2shoot->mapFromGlobal(cursor.pos());
            pt.setX(1);
            pt.setY(1);
            painter.drawImage(pt, cursorImg);
        }
        else
        {
            helpers::log("ScreenShooter:onMakeScreenshot ... mouse not in win", LOG_INF, qApp, 0);
        }
    }*/
    m_Screenshots.append(bmp);
    if(!bmp.save(sfn))
    {
        helpers::log("ScreenShooter:onMakeScreenshot '"+sfn+"'", LOG_ERR, qApp, 0);
        qDebug() << QImageWriter::supportedImageFormats();
    }
    else
    {
        m_ScreenshotPaths.append(sfn);
    }
}
void ScreenShooter::start(QWidget* w2shoot)
{
    stop();

    m_Screenshots.empty();
    m_ScreenshotPaths.empty();

    this->m_w2shoot = w2shoot;

    static QString sDir = qApp->applicationDirPath()+QDir::separator()+"gifmovie";
    int iDeletedFiles = helpers::emptydir(sDir, qApp, 0, "*.png", false) + helpers::emptydir(sDir, qApp, 0, "*.gif", false);
    if( iDeletedFiles > 0)
        helpers::log("ScreenShooter:start dir emptied("+QString::number(iDeletedFiles)+"):"+sDir, LOG_WRN, qApp, 0);

    this->onMakeScreenshot();
    m_pTimer = new QTimer();
    QObject::connect(m_pTimer,SIGNAL(timeout()),this,SLOT(onMakeScreenshot()));
    m_pTimer->start(500);
}
bool ScreenShooter::save(CxImage* img, CxImageGIF* pimg, QString sAbsFN, CxImage** pimages, int pimagesSize)
{
    QFile::remove(sAbsFN);
    wchar_t* wc =new wchar_t[sAbsFN.size()+1];
    sAbsFN.toWCharArray(wc);
    wc[sAbsFN.size()] = 0;
    CxIOFile hFile;
    if(!hFile.Open(wc, _T("wb")))
    {
        helpers::log("ScreenShooter:save: !open:"+sAbsFN, LOG_ERR, qApp, 0);
        return false;
    }
    delete[] wc;
    int mode = sAbsFN.endsWith(".gif") ? CXIMAGE_FORMAT_GIF : CXIMAGE_FORMAT_PNG;
    if(pimg != 0)
    {
        if(!pimg->Encode(&hFile,pimages,pimagesSize,true,false))
        {
            helpers::log("ScreenShooter:save: !encode.multi "+sAbsFN, LOG_ERR, qApp, 0);
        }
        hFile.Close();
    }
    else
    {
        if(!img->Encode(&hFile,mode))
        {
            helpers::log("ScreenShooter:save: !encode "+sAbsFN, LOG_ERR, qApp, 0);
            hFile.Close();
            return false;
        }
    }
    if(!hFile.Close())
    {
        helpers::log("ScreenShooter:save: !close "+sAbsFN, LOG_ERR, qApp, 0);
    }
    return true;
}
QString ScreenShooter::GetAsGif(QString sAbsPicFN)
{
    sAbsPicFN = sAbsPicFN.replace('/', '\\');

    static QString sExe = qApp->applicationDirPath()+QDir::separator()+"imgmagick" + QDir::separator() + "convert.exe";
    sExe = sExe.replace('/', '\\');

    if(!QFile::exists(sExe))
    {
        helpers::log("ScreenShooter:GetAsGif: FNF:"+sExe, LOG_ERR, qApp, 0);
    }

    QStringList arguments;
    QString sAbsPicFNNew = helpers::f_replaceext(sAbsPicFN, "gif");
    QFile::remove(sAbsPicFNNew);
    arguments << sAbsPicFN << sAbsPicFNNew;
    QProcess p;
    p.setWorkingDirectory(qApp->applicationDirPath()+QDir::separator()+"imgmagick");
    int i = p.execute(sExe, arguments); //if the process cannot be started, -2 is returned

    if(!QFile::exists(sAbsPicFNNew))
    {
        helpers::log("ScreenShooter:GetAsGif: !converta: return value:"+QString::number(i), LOG_ERR, qApp, 0);
        helpers::log("ScreenShooter:GetAsGif: !convertb:"+sExe, LOG_ERR, qApp, 0);
        helpers::log("ScreenShooter:GetAsGif: !convertc:"+sAbsPicFN, LOG_ERR, qApp, 0);
        helpers::log("ScreenShooter:GetAsGif: !convertd:"+sAbsPicFNNew, LOG_ERR, qApp, 0);
    }

    return sAbsPicFNNew;
}

void ScreenShooter::createani()
{
    stop();

//TODO: progress bar + MsgBox in case of error/finish

    CxImage* pimage[99999]; // = CxImage* pimage = new CxImage*[list.size()];
    for(int i=0;i<m_ScreenshotPaths.size();i++)
    {
        CxImage* newImage = new CxImage();
        QString s = GetAsGif(m_ScreenshotPaths.at(i));
        wchar_t* wc =new wchar_t[s.size()+1];       //yes, yes, I know: I would have to cleanup
        s.toWCharArray(wc);
        wc[s.size()] = 0;
        int mode = s.endsWith(".gif") ? CXIMAGE_FORMAT_GIF : CXIMAGE_FORMAT_PNG;
        if(!newImage->Load(wc, mode))
        {
            helpers::log("ScreenShooter:createani: !load:"+s, LOG_ERR, qApp, 0);
        }
        //newImage->IncreaseBpp(8);
        newImage->SetFrameDelay(35);
        pimage[i] = newImage;
        delete[] wc;
    }
    {
        CxImageGIF multiimage;
        multiimage.SetComment("animated gif - GHE");
        //multiimage.SetLoops(3);
        multiimage.SetDisposalMethod(2);
        QString s = qApp->applicationDirPath()+QDir::separator()+"gifmovie"+QDir::separator()+"r.gif";
        save(0, &multiimage, s, pimage, m_ScreenshotPaths.size());
    }
}
void ScreenShooter::stop()
{
    if(m_pTimer)
    {
        m_pTimer->stop();
        delete m_pTimer;
    }

    m_pTimer = 0;
    m_w2shoot = 0;
    m_iScreenshot = 0;
}
