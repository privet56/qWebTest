/********************************************************************************************************
 * PROGRAM      : childform
 * DATE - TIME  : Samstag 30 Dezember 2006 - 12h04
 * AUTHOR       :  (  )
 * FILENAME     : QSkinObject.cpp
 * LICENSE      : 
 * COMMENTARY   : 
 ********************************************************************************************************/
#include <QtGui>
#include <cmath>

#include "qskinobject.h"



QSkinObject::QSkinObject( QWidget* wgtParent)
    : QObject()
   
{
	// Widget which will be skinned
	skinWidget = wgtParent;
	styleRenderer = 0;
	contentsRect = skinWidget->rect();
	// Save flags for later unsset the skin
	flags = skinWidget->windowFlags();
	// do some initialisation
	skinTimer = new QBasicTimer();
	widgetMask = QPixmap(skinWidget->width()+12, skinWidget->height()+12);
	mousePress = false;	
}

void QSkinObject::startSkinning()
{
	loadSkinIni();
	QPoint pos = skinWidget->pos();
	skinWidget->setAttribute(Qt::WA_TranslucentBackground);
	//no windows frame
	skinWidget->setWindowFlags(Qt::FramelessWindowHint);
	//loadSkinIni();
	
	
	//mouse tracking on, because the close-button should glow if the cursor comes over it
	skinWidget->setMouseTracking(true);
	
	QRectF rect_top = styleRenderer->boundsOnElement("border_top");
	QRectF rect_right = styleRenderer->boundsOnElement("border_right");
	QRectF rect_bottom = styleRenderer->boundsOnElement("border_bottom");
	QRectF rect_left = styleRenderer->boundsOnElement("border_left");
	
	skinWidget->setContentsMargins ( rect_left.width(), rect_top.height(), rect_right.width(), rect_bottom.height());
	
	skinWidget->installEventFilter(this);
	resizeHandler =  new QSkinWidgetResizeHandler(this, skinWidget);
	setAlphaValue(alpha);
	skinTimer->start(100, this);
	skinWidget->show();
	skinWidget->move(pos);
}

void QSkinObject::stopSkinning()
{
	skinWidget->setContentsMargins ( 5, 5, 5, 5);
	skinWidget->clearMask();
	skinWidget->setAttribute(Qt::WA_TranslucentBackground, false);
	skinWidget->setWindowFlags(flags);
	skinWidget->removeEventFilter(this);
	skinWidget->removeEventFilter(resizeHandler);
	skinTimer->stop();
	skinWidget->setMouseTracking(false);
	setAlphaValue(1.0);
	skinWidget->show();
}
bool QSkinObject::eventFilter(QObject *o, QEvent *e)
{
	
	if(e->type() == QEvent::MouseButtonRelease)
	{
		mouseReleaseEvent(static_cast<QMouseEvent*>(e));
		return false;
	}
	else if(e->type() == QEvent::Resize)
	{
		resizeEvent(static_cast<QResizeEvent*>(e));
		return false;
	}
	else if(e->type() == QEvent::Paint)
	{
		paintEvent(static_cast<QPaintEvent*>(e));
		return false;
	}
	/*else if(e->type() == QEvent::Close)
	{
		closeEvent(static_cast<QCloseEvent*>(e));
		return false;
	}*/
	else
	{
		 //resizeHandler->eventFilter(o, e);
		 return skinWidget->eventFilter(o, e);
	}
}

void QSkinObject::manageRegions()
{
	QRectF rect_top = styleRenderer->boundsOnElement("border_top");
	QRectF rect_topright = styleRenderer->boundsOnElement("border_topright");
	QRectF rect_min = styleRenderer->boundsOnElement("minimize_button_normal");
	QRectF rect_max = styleRenderer->boundsOnElement("maximize_button_normal");
	QRectF rect_close = styleRenderer->boundsOnElement("close_button_normal");

	quitButton = QRegion( QRect(skinWidget->width()-rect_topright.width() - rect_close.width(), (rect_top.height() - rect_close.height()) / 2, rect_close.width(), rect_close.height()) );
	maxButton = QRegion( QRect(skinWidget->width()-rect_topright.width() - rect_close.width() - rect_max.width(), (rect_top.height() - rect_max.height()) / 2, rect_max.width(), rect_max.height()) );
	minButton = QRegion( QRect(skinWidget->width()-rect_topright.width() - rect_close.width() - rect_min.width() - rect_max.width(), (rect_top.height() - rect_min.height()) / 2, rect_min.width(), rect_min.height()) );
}

/*void QSkinObject::closeEvent(QCloseEvent *e)
{

}*/

void QSkinObject::resizeEvent(QResizeEvent *e)
{
	manageRegions();
	
	e->ignore();
	//skinWidget->resizeEvent(e);
}

void QSkinObject::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) 
    {
		mousePress = true;
	}
}
void QSkinObject::mouseReleaseEvent(QMouseEvent *e)
 {
	// winwow Mover
	QRectF rect_top = styleRenderer->boundsOnElement("border_top");
	QRectF rect_topright = styleRenderer->boundsOnElement("border_topright");
	QRectF rect_min = styleRenderer->boundsOnElement("minimize_button_normal");
	QRectF rect_max = styleRenderer->boundsOnElement("maximize_button_normal");
	QRectF rect_close = styleRenderer->boundsOnElement("close_button_normal");
	QPoint pos = e->pos();
	
	//QRect rect = skinWidget->rect();
    if (e->button() == Qt::LeftButton) 
    {       
		// buttons
		mousePress = false;
		int button = -1;
		int butPos = (rect_top.height() - rect_close.height()+6);
		if(quitButton.contains(e->pos()))
		 button = 0 ;
		else if(maxButton.contains(e->pos()))
		 button = 1 ;
		else if(minButton.contains(e->pos()))
		 button = 2 ;
		if(pos.x() < skinWidget->width() - 5)
			if(pos.y() < rect_top.height() + butPos && pos.y() > 6)
			{
				switch (button) {
					case 0:
					skinWidget->close();
					 e->accept();
					 break;
					case 1:
					 if(skinWidget->isMaximized())
						skinWidget->showNormal();
					 else
						skinWidget->showMaximized();
					 e->accept();
					 break;
					case 2: 
					 skinWidget->showMinimized();
					 e->accept();
					 break;
				}
			}
			else
				e->ignore();
		
    }

 }
/*void QSkinObject::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	
}
void QSkinObject::mouseMoveEvent(QMouseEvent *e)
{
	
}*/

void QSkinObject::setSkinPath(const QString & skinpath)
{
	skinPath = skinpath;
	skinWidget->update();
	
}
QString QSkinObject::getSkinPath()
{
	return skinPath;
}
void QSkinObject::updateButtons()
{
	QPainter p(&widgetMask);
	p.setRenderHint(QPainter::Antialiasing);
	
	QRectF rect_top = styleRenderer->boundsOnElement("border_top");
	QRectF rect_topright = styleRenderer->boundsOnElement("border_topright");
	QRectF rect_min = styleRenderer->boundsOnElement("minimize_button_normal");
	QRectF rect_max = styleRenderer->boundsOnElement("maximize_button_normal");
	QRectF rect_close = styleRenderer->boundsOnElement("close_button_normal");
	
	QString state_min, state_max, state_close;
	if(minButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
	{
		if(!mousePress)
			state_min = "minimize_button_hover";
		else
			state_min = "minimize_button_pressed";
	}
	else
	{
		state_min = "minimize_button_normal";
	}

    //if(mousePress) qDebug("pressed");

	if(quitButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
	{
		if(!mousePress)
			state_close = "close_button_hover";
		else
			state_close = "close_button_pressed";
	}
	else
	{
		state_close = "close_button_normal";
	}

	if(maxButton.contains(skinWidget->mapFromGlobal(QCursor::pos())))
	{
		if(!mousePress)
			state_max = "maximize_button_hover";
		else
			state_max = "maximize_button_pressed";
	}
	else
	{
		state_max = "maximize_button_normal";
	}
	styleRenderer->render(&p, state_min, 
						  QRectF(skinWidget->width() - rect_min.width() - rect_close.width() - rect_max.width() - rect_topright.width(),
								 (rect_top.height() - rect_min.height()) / 2,
								 rect_min.width(), rect_min.height()));
								 
	styleRenderer->render(&p, state_max, 
						  QRectF(skinWidget->width() - rect_close.width() - rect_max.width() - rect_topright.width(),
								 (rect_top.height() - rect_max.height()) / 2,
								 rect_max.width(), rect_max.height()));
								 
	styleRenderer->render(&p, state_close, 
						  QRectF(skinWidget->width() - rect_close.width() - rect_topright.width(),
								 (rect_top.height() - rect_close.height()) / 2,
								 rect_close.width(), rect_close.height()));
	p.end();
}
void QSkinObject::updateStyle()
{
	QRectF rect_topleft = styleRenderer->boundsOnElement("border_topleft");
	QRectF rect_top = styleRenderer->boundsOnElement("border_top");
	QRectF rect_topright = styleRenderer->boundsOnElement("border_topright");
	QRectF rect_right = styleRenderer->boundsOnElement("border_right");
	QRectF rect_bottomright = styleRenderer->boundsOnElement("border_bottomright");
	QRectF rect_bottom = styleRenderer->boundsOnElement("border_bottom");
	QRectF rect_bottomleft = styleRenderer->boundsOnElement("border_bottomleft");
	QRectF rect_left = styleRenderer->boundsOnElement("border_left");
	QRectF rect_close = styleRenderer->boundsOnElement("close_button_normal");
	int top_width = skinWidget->width() - rect_topleft.width() - rect_topright.width();
	int left_height = skinWidget->height() - rect_topleft.height() - rect_bottomleft.height();
	int right_height = skinWidget->height() - rect_topright.height() - rect_bottomright.height();
	int bottom_width = skinWidget->width() - rect_bottomleft.width() - rect_bottomright.width();	
	
	skinWidget->setMinimumSize (rect_left.width() + rect_right.width() + 128, rect_top.height() + rect_bottom.height());
	
	widgetMask = QPixmap(skinWidget->width(), skinWidget->height());
	widgetMask.fill(Qt::transparent);

	QPainter p(&widgetMask);
		
	

	styleRenderer->render(&p, "border_topleft", 
						  QRectF(0,0,rect_topleft.width(), rect_topleft.height()));
	styleRenderer->render(&p, "border_top", 
						  QRectF(rect_topleft.width(),0,
								 top_width, rect_topleft.height()));
	styleRenderer->render(&p, "border_topright", 
						  QRectF(top_width + rect_topright.width(),0,rect_topright.width(), rect_topright.height()));
						
	
	styleRenderer->render(&p, "border_left", 
						  QRectF(0,rect_topleft.height(),rect_left.width(), left_height));
	styleRenderer->render(&p, "border_right", 
						  QRectF(bottom_width + rect_bottomright.width(),rect_topright.height(),rect_right.width(), right_height));
						  

	styleRenderer->render(&p, "border_bottomleft", 
						  QRectF(0,left_height + rect_topleft.height(),rect_bottomleft.width(), rect_bottomleft.height()));
	styleRenderer->render(&p, "border_bottom", 
						  QRectF(rect_bottomleft.width(),left_height + rect_topleft.height(),
								 bottom_width, rect_bottomleft.height()));
	styleRenderer->render(&p, "border_bottomright", 
						  QRectF(bottom_width + rect_bottomright.width(),left_height + rect_topright.height(),rect_bottomright.width(), rect_bottomright.height()));	
	
	p.setFont(titleFont);
	p.setPen(QPen(titleColor, 2));
	
	contentsRect = QRect(rect_left.width(), rect_top.height(), skinWidget->width() - rect_right.width() - rect_left.width(), skinWidget->height() - rect_top.height() - rect_bottom.height());
	p.setBrush(backgroundColor);
	p.setPen(QPen(Qt::black, 1));
	p.drawRect(contentsRect);
	p.drawText(QRect(rect_left.width(), 0, skinWidget->width()-rect_close.width()*3-8-rect_left.width()-rect_right.width(), rect_top.height()), Qt::AlignVCenter, skinWidget->windowTitle());
	p.end();
	skinWidget->setMask(widgetMask.mask());
	
	updateButtons();
}

void QSkinObject::loadSkinIni()
{
	QSettings s(QApplication::applicationDirPath() + "/skin/skin.dat", QSettings::IniFormat);
	s.beginGroup("Skin");

    //backgroundColor = QColor(s.value("Hintergrundfarbe", Qt::white).toString());
    backgroundColor = QColor(s.value("Hintergrundfarbe", QColor(255,255,255)).toString());

	titleColor = QColor(s.value("Titelfarbe", QColor(255,100,0)).toString());
	alpha = s.value("Alpha", 1.0).toDouble();
	QFont font;
	font.fromString(s.value("Titelschrift", QFont("Arial")).toString());
	skinPath = QApplication::applicationDirPath() + "/" + s.value("Skinpfad", QApplication::applicationDirPath() + "skin/template.svg").toString();
	titleFont = font;
	styleRenderer = new QSvgRenderer(skinPath);
	s.endGroup();
}
void QSkinObject::paintEvent(QPaintEvent *event)
{
	if(skinWidget)
	{
		updateStyle();
		QPainter p(skinWidget);
		p.setRenderHint(QPainter::Antialiasing);
		//p.setOpacity((qreal)(alpha/255));
		
		//skinWidget->setWindowOpacity((qreal)(alpha/255));
		p.drawPixmap(0,0,widgetMask);
		p.end();
	}
}

void QSkinObject::timerEvent ( QTimerEvent * event )
{
	if(event->timerId() == skinTimer->timerId())
		skinWidget->repaint();
}

int QSkinObject::customFrameWidth()
{
	QRectF rect_left = styleRenderer->boundsOnElement("border_left");
	return rect_left.width();
}

