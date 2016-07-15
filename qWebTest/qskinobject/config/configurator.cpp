#include "configurator.h"
Configurator::Configurator(QWidget *parent)
   : QWidget(parent)
{
     setupUi(this); 
     qskinobject = new QSkinObject(this);
	 
	 connect(tcs, SIGNAL(clicked()),this,SLOT(changeTCS()));
     connect(bcs, SIGNAL(clicked()),this,SLOT(changeBCS()));  
	 connect(fs, SIGNAL(clicked()),this,SLOT(changeFS()));
	 connect(skinon, SIGNAL(stateChanged(int)),this,SLOT(skinOnToggled(int)));
	 connect(erstellen, SIGNAL(clicked()),this,SLOT(Erstellen())); 
	 connect(doubleSpinBox, SIGNAL(valueChanged(double)),qskinobject,SLOT(setAlphaValue(double))); 
	initData();
	if(skinon->isChecked())
		qskinobject->startSkinning();
	
}  
void Configurator::skinOnToggled(int skio)
{
	if(skio == Qt::Checked)
		qskinobject->startSkinning();
	else 
	 	qskinobject->stopSkinning();
}
void Configurator::Erstellen()
{
	 
	QSettings writer(QApplication::applicationDirPath() + "/skin/skin.dat", QSettings::IniFormat);
	writer.beginGroup("Skin");
    writer.setValue("Hintergrundfarbe", BCS);
	writer.setValue("Titelfarbe", TCS);
	writer.setValue("Titelschrift", FS);
    writer.setValue("Alpha", doubleSpinBox->value());
	
	writer.setValue("SkinOn", skinon->isChecked());
	writer.setValue("Skinpfad", "skin/" + skinList->currentItem()->text());
    writer.endGroup();
	QString htmlText =
		"<p><b><font color=" + BCS.name() + ">Hintergrundfarbe</font></b></p>"
		"<p><b><font color=" + TCS.name() + ">Titelfarbe</font></b></p>"
		"<p><b><font face="  + FS.family()+">Titelschrift : " + FS.family() +"</b></p>" + 
		"<p><b>Skin : " + skinList->currentItem()->text() +"</b></p>";
	QMessageBox::about(this, "Skin Einstellungen :", htmlText);
	qskinobject->startSkinning();
	
}

void Configurator::initData()
{
	initSkinList();
	QSettings settings(QApplication::applicationDirPath() + "/skin/skin.dat", QSettings::IniFormat);
	settings.beginGroup("Skin");
	
    doubleSpinBox->setValue(settings.value("Alpha", 1.9).toDouble());
	skinon->setChecked(settings.value("SkinOn").toBool());
    BCS = QColor(settings.value("Hintergrundfarbe").toString());
	TCS = settings.value("Titelfarbe").toString();
	QFont font;
	font.fromString(settings.value("Titelschrift").toString());
	FS = font;
	QString sp = settings.value("Skinpfad").toString().remove("skin/");
    settings.endGroup();
	fs->setFont(FS);
	QPixmap pxm(32,32);
	pxm.fill(BCS);
	bcs->setIcon(QIcon(pxm));
	pxm.fill(TCS);
	tcs->setIcon(QIcon(pxm));
	sp.resize(sp.length() -1);
	for(int i = 0; i < skinList->count(); ++i)
		if(skinList->item(i)->text() == sp)
		{
			skinList->setCurrentRow(i);
			
		}
		else
			skinList->setCurrentRow(0);
		
}
void Configurator::initSkinList()
{	
	QDir dir(QApplication::applicationDirPath() + "/skin/");
	QFileInfoList skinDirs = dir.entryInfoList(QStringList() << "*.svg" << "*.svgz", QDir::NoDotAndDotDot | QDir::Files);
	foreach(QFileInfo i, skinDirs)
	{
		QPixmap pix(24,24);
		pix.fill(Qt::blue);
		QListWidgetItem *item = new QListWidgetItem(QIcon(pix), i.fileName());
		skinList->addItem(item);
	}
}
