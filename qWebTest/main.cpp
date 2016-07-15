#include <windows.h>
#include "ximage.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include "qskinobject/qskinobject/qskinobject.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool bStyleIt = false;
    {
        QFile data(qApp->applicationDirPath() + "/css.css");
        QString style;
        if (data.open(QFile::ReadOnly))
        {
            QTextStream styleIn(&data);
            style = styleIn.readAll();
            data.close();
            a.setStyleSheet(style);
            bStyleIt = true;
        }
    }

    MainWindow w;
    if(bStyleIt)
    {
        QSkinObject *sw = new QSkinObject(&w);
        sw->startSkinning();
    }
    w.show();

    return a.exec();
}
