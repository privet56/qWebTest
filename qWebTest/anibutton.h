#ifndef ANIBUTTON_H
#define ANIBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QMovie>
#include "helpers.h"
#include <QDebug>

class anibutton : public QPushButton
{
    Q_OBJECT
public:
    explicit anibutton(QWidget *parent = 0);

    bool setani(QString sRes);

signals:

public slots:
    void setButtonIcon(int frame);

};

#endif // ANIBUTTON_H
