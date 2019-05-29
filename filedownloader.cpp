#include "filedownloader.h"
#include <iostream>

FileDownloader::FileDownloader(QUrl imageUrl)
{
    this->imageUrl = imageUrl;
    flag = true;
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply *pReply)
{
    //std::cout << "fileDownloaded" << std::endl;
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded();

}

QByteArray FileDownloader::downloadedData() const
{
    //std::cout << "downloadedData" << std::endl;
    return m_DownloadedData;
}

void FileDownloader::run()
{
    m_WebCtrl = new QNetworkAccessManager();
    connect(m_WebCtrl, SIGNAL(finished(QNetworkReply *)),
        this, SLOT(fileDownloaded(QNetworkReply *)));
    while (flag)
    {
    m_WebCtrl->get(QNetworkRequest(imageUrl));
    this->exec();
    }
}

void FileDownloader::Stop()
{
    this->quit();
    flag = false;
    if (!this->wait(5000))
    {
    qWarning("FileDownloader : Thread deadlock detected");
    this->terminate();
    this->wait();
    }
}
