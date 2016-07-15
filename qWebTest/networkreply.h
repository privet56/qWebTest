#ifndef NETWORKREPLY_H
#define NETWORKREPLY_H

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QSet>
#include "helpers.h"

class networkReply : public QNetworkReply
{
    Q_OBJECT
public:
    networkReply(QObject *parent);

signals:

public slots:

};

#endif // NETWORKREPLY_H
