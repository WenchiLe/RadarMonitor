#include "filedownloader.h"
#include <iostream>

FileDownloader::FileDownloader(QUrl imageUrl)
{
    this->imageUrl = imageUrl;
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
    std::cout << "downloadedData" << std::endl;
    return m_DownloadedData;
}

void FileDownloader::run()
{
    m_WebCtrl = new QNetworkAccessManager();
    connect(
    m_WebCtrl, SIGNAL(finished(QNetworkReply *)),
    this, SLOT(fileDownloaded(QNetworkReply *))
    );
    while (true)
    {
    m_WebCtrl->get(QNetworkRequest(imageUrl));
    this->exec();
    }
}
