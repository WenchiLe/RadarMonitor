#ifndef GETLICENSEPLATETHREAD_H
#define GETLICENSEPLATETHREAD_H

#include <QObject>
#include <QThread>
#include <receivelicenseplate.h>
#include <QVector>
#include <QList>

class GetLicensePlateThread : public QThread
{
    Q_OBJECT

public:
    GetLicensePlateThread();
    void Stop();

signals:
void LicensePlateChanged(ReceiveLicensePlate::carLicense carLicense);//signal to draw the last frame

protected:
    void run();

private:
    ReceiveLicensePlate receiveLicensePlate;
    bool flag;
};

#endif // GETLICENSEPLATETHREAD_H
