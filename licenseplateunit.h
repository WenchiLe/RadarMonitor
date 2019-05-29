#ifndef LICENSEPLATEUNIT_H
#define LICENSEPLATEUNIT_H

#include <QtCore>
#include <QMutex>
#include <receivedatafromserver.h>
#include <QQueue>

class LicensePlateUnit
{
public:
    struct carLicense
    {
        double longtitude;
        double latitude;
        int64_t time;
        double speed;
        QString license;
    };
    QQueue<carLicense> queue_carLicense;

public:
    LicensePlateUnit();
    void PushNewFrame(ReceiveDataFromServer::CarLicense carLicense);
    carLicense FetchFrame();
    bool HasFrame();
};

#endif // LICENSEPLATEUNIT_H
