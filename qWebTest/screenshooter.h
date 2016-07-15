#ifndef SCREENSHOOTER_H
#define SCREENSHOOTER_H

#include <QObject>
#include <QMovie>
#include <QTimer>
#include <QWidget>
#include <QPushButton>
#include <QMovie>
#include "helpers.h"
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QImageReader>
#include <QImageWriter>
#include <QCursor>
#include <QPixmap>
#include <QImage>
#include <QBitmap>
#include "ximage.h"
#include "ximagif.h"
#include "ximapng.h"

class ScreenShooter : public QObject
{
    Q_OBJECT
public:
    static bool canScreenShoot();
    explicit ScreenShooter(QObject *parent = 0);
    virtual ~ScreenShooter();

    QTimer* m_pTimer;
    int m_iScreenshot;
    QList<QImage> m_Screenshots;
    QStringList m_ScreenshotPaths;
    QWidget* m_w2shoot;

    void start(QWidget* w2shoot);
    void createani();
    void stop();
    bool save(CxImage* img, CxImageGIF* pimg, QString sAbsFN, CxImage** pimages=0, int pimagesSize=0);
    QString GetAsGif(QString sAbsPicFN);

signals:

public slots:
    void onMakeScreenshot();

};

#endif // SCREENSHOOTER_H
