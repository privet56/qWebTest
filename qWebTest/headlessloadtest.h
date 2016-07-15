#ifndef HEADLESSLOADTEST_H
#define HEADLESSLOADTEST_H

#include <QObject>
#include <QSet>
#include "headlesswebpage.h"

class HeadlessLoadTest : public QObject
{
    Q_OBJECT
public:
    explicit HeadlessLoadTest(QObject *parent = 0);
    void loadtest();

    QSet<HeadlessWebPage*> m_pagesWorking;
    QSet<HeadlessWebPage*> m_pagesFinished;

    QMutex mutex;

signals:

public slots:
    void testFinished(HeadlessWebPage* hwp);
};

#endif // HEADLESSLOADTEST_H
