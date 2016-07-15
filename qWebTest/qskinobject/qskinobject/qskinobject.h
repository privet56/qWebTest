/********************************************************************************************************
 * PROGRAM      : childform
 * DATE - TIME  : Samstag 30 Dezember 2006 - 12h04
 * AUTHOR       :  (  )
 * FILENAME     : QSkinMainWindow.h
 * LICENSE      : 
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef QSkinObject_H
#define QSkinObject_H
#include "qskinwidgetresizehandler.h"
#include <QtGui>
#include <QtSvg>

class QSkinWidgetResizeHandler;
class  QSkinObject : public QObject
{
    Q_OBJECT
    friend class QSkinWidgetResizeHandler;
public:
    	QSkinObject(QWidget* wgtParent);
	~QSkinObject(){}
	void setSkinPath(const QString & skinpath);
	QString getSkinPath();
	int customFrameWidth();
public slots:
	void updateStyle();
	void updateButtons();
	void startSkinning();
	void stopSkinning();
	void setAlphaValue(double a){alpha = a; skinWidget->setWindowOpacity(alpha);}
protected:
	bool eventFilter(QObject *o, QEvent *e);
	//Events to filter
	//void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *mouseEvent);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *e);
	//void closeEvent(QCloseEvent *e);

	void loadSkinIni();
	void manageRegions();
	void timerEvent ( QTimerEvent * event );
private:
    QPoint dragPosition;
	QPixmap widgetMask;//the pixmap, in which the ready frame is stored 
	QString skinPath;
	QFont titleFont;
	QColor titleColor;
	QColor backgroundColor;
    bool gotMousePress;	
	QRegion quitButton;
	QRegion maxButton;
	QRegion minButton;
	QRect contentsRect;
	QSkinWidgetResizeHandler * resizeHandler;
	bool mousePress;
	QBasicTimer *skinTimer;
	QWidget *skinWidget;
	Qt::WindowFlags flags;
private:
	qreal alpha;
	QSvgRenderer *styleRenderer;
};
#endif

