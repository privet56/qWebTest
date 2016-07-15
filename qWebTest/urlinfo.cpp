#include "urlinfo.h"

UrlInfo::UrlInfo(QString url, QString content, int iTestIndex, QObject *parent) : QObject(parent)
{
    this->m_url = url;
    this->m_iTestIndex = iTestIndex;
    this->m_content = content;
}
