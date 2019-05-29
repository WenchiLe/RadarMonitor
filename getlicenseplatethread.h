#ifndef GETLICENSEPLATETHREAD_H
#define GETLICENSEPLATETHREAD_H

#include <QObject>
#include <QThread>
#include <receivelicenseplate.h>
#include <receivedatafromserver.h>
#include <QVector>
#include <QList>

class GetLicensePlateThread : public QThread
{
    Q_OBJECT

public:
    GetLicensePlateThread(ReceiveDataFromServer *receiveDataFromServer);
    void Stop();

signals:
void LicensePlateChanged(ReceiveDataFromServer::CarLicense carLicense);

protected:
    void run();

private:
    //ReceiveLicensePlate receiveLicensePlate;
    bool flag;
    ReceiveDataFromServer *receiveDataFromServer;
};

#endif // GETLICENSEPLATETHREAD_H
