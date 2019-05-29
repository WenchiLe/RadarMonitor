#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H


#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QThread
{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl imageUrl);
    virtual ~FileDownloader();
    QByteArray downloadedData() const;
    void Stop();

protected:
    void run();

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply *pReply);

private:
    QUrl imageUrl;
    QNetworkAccessManager *m_WebCtrl;
    //QNetworkRequest *request;
    QByteArray m_DownloadedData;
    bool flag;
};

#endif // FILEDOWNLOADER_H
