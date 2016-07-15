#ifndef URLINFO_H
#define URLINFO_H

#include <QObject>

class UrlInfo : public QObject
{
    Q_OBJECT
public:
    explicit UrlInfo(QString url, QString content, int iTestIndex, QObject *parent = 0);

    QString m_url;
    QString m_content;
    int m_iTestIndex;

signals:

public slots:

};

#endif // URLINFO_H
