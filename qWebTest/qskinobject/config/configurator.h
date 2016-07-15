#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_
#include <QtGui>
#include "ui_configurator.h"
#include "../qskinobject/qskinobject.h"
class Configurator : public QWidget, public Ui::configurator
{
      Q_OBJECT
public:
       Configurator(QWidget *parent = 0);
	   void initData();
public slots:
	   void skinOnToggled(int skio);
	   void initSkinList();
	   void Erstellen();
	   void changeBCS()
	   {
			BCS = QColorDialog::getColor(Qt::white, this);
			QPixmap pxm(32,32);
			pxm.fill(BCS);
			bcs->setIcon(QIcon(pxm));
	  }
	  void changeTCS()
	  {
			TCS = QColorDialog::getColor(Qt::black, this);
			QPixmap pxm(32,32);
			pxm.fill(TCS);
			tcs->setIcon(QIcon(pxm));
	  }
	  void changeFS()
	  {
	  bool ok;
	  FS = QFontDialog::getFont(
                 &ok, QFont("Comic Sans MS", 8), this);
	  if (ok)
		fs->setFont(FS);
	  }
private:
	QSkinObject *qskinobject;
	QColor BCS,TCS;
	QFont FS;
	
	
};
#endif

