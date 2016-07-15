#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include "ximage.h"
#include "qt_windows.h"
#include <QMainWindow>
#include <QMovie>
#include <QTimer>
#include "screenshooter.h"
#include "ximage.h"
#include "ximagif.h"
#include "ximapng.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setRoundedCorners(int radius_tl, int radius_tr, int radius_bl, int radius_br);

    ScreenShooter* m_screenshooter;
    void load(QString sUrl);

public slots:

    void oninit();

    void on_lineEdit_returnPressed();

    void on_webView_objectNameChanged(const QString &objectName);

    void on_webView_iconChanged();

    void on_webView_linkClicked(const QUrl &arg1);

    void on_webView_loadFinished(bool arg1);

    void on_webView_loadStarted();

    void on_webView_titleChanged(const QString &title);

    void on_webView_urlChanged(const QUrl &arg1);

    void on_webView_windowIconChanged(const QIcon &icon);

    void on_webView_windowTitleChanged(const QString &title);

    void on_webView_windowIconTextChanged(const QString &iconText);

    void on_pushButton_toggled(bool checked);

    void log(QString s, int iLevel, QApplication* pApp, bool bWithDate);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
