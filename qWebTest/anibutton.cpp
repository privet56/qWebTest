#include "anibutton.h"

anibutton::anibutton(QWidget *parent) : QPushButton(parent)
{

}

bool anibutton::setani(QString sRes)
{
    QMovie* pMovie = new QMovie(sRes);
    connect(pMovie,SIGNAL(frameChanged(int)),this,SLOT(setButtonIcon(int)));
    if (pMovie->loopCount() != -1) //if movie doesn't loop forever, force it to
        connect(pMovie,SIGNAL(finished()),pMovie,SLOT(start()));
    pMovie->start();
    if(!pMovie->isValid())
    {
        helpers::log("anibutton:setani NOT.ok("+sRes+"):"+QString::number(pMovie->isValid()), LOG_WRN, qApp, 0);
        qDebug()<<QImageReader::supportedImageFormats();
        return false;
    }
    return true;
}

void anibutton::setButtonIcon(int frame)   // member function that catches the frameChanged signal of the QMovie
{
    if(!this->isChecked())return;
    Q_UNUSED(frame)
    QMovie* pMovie = (QMovie*)sender();
    this->setIcon(QIcon(pMovie->currentPixmap()));
}
