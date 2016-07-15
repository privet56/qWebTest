#run qmake after EVERY change of the pro file manually ('build all' is not enough)

QT += core gui webkitwidgets webkit opengl sql svg xml xmlpatterns multimedia network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webtest
TEMPLATE = app

#DEFINES += QT_DLL QT_NETWORK_LIB QT_WIDGETS_LIB QT_WEBKITWIDGETS_LIB

DEFINES += UNICODE
DEFINES += _UNICODE
DEFINES += NOMINMAX
DEFINES += CXIMAGE_SUPPORT_GIF
DEFINES += CXIMAGE_SUPPORT_PNG

INCLUDEPATH += ./

SOURCES += main.cpp\
        mainwindow.cpp \
    webview.cpp \
    webpage.cpp \
    helpers.cpp \
    networkaccessmanager.cpp \
    networkreply.cpp \
    headlessloadtest.cpp \
    headlesswebpage.cpp \
    urlinfo.cpp \
    headlesswebpagelogger.cpp \
    printwebview.cpp \
    printwebpage.cpp \
    collectmailsoffice365.cpp \
    collectmailsmongo.cpp \
    urldownloader.cpp \
    urldownloaderwp.cpp \
    collectmailsesb.cpp \
    xml.cpp \
    anibutton.cpp \
    screenshooter.cpp \
    qskinobject/config/configurator.cpp \
    qskinobject/qskinobject/qskinobject.cpp \
    qskinobject/qskinobject/qskinwidgetresizehandler.cpp \
    doqter.cpp \
    ssh.cpp

HEADERS  += mainwindow.h \
    resource.h \
    webview.h \
    webpage.h \
    helpers.h \
    networkaccessmanager.h \
    networkreply.h \
    headlessloadtest.h \
    headlesswebpage.h \
    urlinfo.h \
    headlesswebpagelogger.h \
    printwebview.h \
    printwebpage.h \
    collectmailsoffice365.h \
    collectmailsmongo.h \
    urldownloader.h \
    urldownloaderwp.h \
    collectmailsesb.h \
    xml.h \
    anibutton.h \
    screenshooter.h \
    qskinobject/config/configurator.h \
    qskinobject/qskinobject/qskinobject.h \
    qskinobject/qskinobject/qskinwidgetresizehandler.h \
    doqter.h \
    ssh.h

FORMS    += mainwindow.ui \
    qskinobject/config/configurator.ui

OTHER_FILES += ico.ico res.rc \
    ico.ico \
    /res/record.gif \
    res.rc \
    css.css \
    qskinobject/skin/blue.svg \
    qskinobject/skin/template.svg \
    qskinobject/skin/skin.dat

RESOURCES += res.qrc

RC_FILE = res.rc

#PRECONDITION: BUILD c:\Qt\_qt_apps\QtMEL_Examples\cximage701_full\CxImgLib.sln WITH RELEASE-UNICODE
#PRECONDITION: qdatetime.h: //Qt5, starting from line 122
#ifdef NOMINMAX
#    static inline qint64 nullJd() { return Q_INT64_C(-784350574879); }
#else
#    static inline qint64 nullJd() { return std::numeric_limits<qint64>::min(); }
#endif

INCLUDEPATH += c:/Qt/_qt_apps/QtMEL_Examples/cximage701_full/CxImage
INCLUDEPATH += c:/Qt/_qt_apps/QtMEL_Examples/cximage701_full/zlib
LIBS += -L"c:/Qt/_qt_apps/QtMEL_Examples/cximage701_full"
LIBS += libpsd/Unicode_Release/libpsd.lib mng/Unicode_Release/mng.lib png/Unicode_Release/png.lib raw/Unicode_Release/libdcr.lib jpeg/Unicode_Release/jpeg.lib zlib/Unicode_Release/zlib.lib tiff/Unicode_Release/tiff.lib jasper/Unicode_Release/jasper.lib cximage/Unicode_Release/cximage.lib

INCLUDEPATH += c:/Qt/_qt_apps/QSsh-master/src/libs/ssh
LIBS += -Lc:/Qt/_qt_apps/build-qssh-Desktop_Qt_5_2_1_MSVC2012_OpenGL_64bit-Release/lib -lBotan -lQSsh
