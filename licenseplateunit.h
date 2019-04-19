#ifndef LICENSEPLATEUNIT_H
#define LICENSEPLATEUNIT_H

#include <QtCore>
#include <QMutex>
#include <receivelicenseplate.h>
#include <QQueue>

class LicensePlateUnit
{
public:
    struct carLicense
    {
        double longtitude;
        double latitude;
        int time;
        QString license;
    };
    QQueue<carLicense> queue_carLicense;

public:
    LicensePlateUnit();
    void PushNewFrame(ReceiveLicensePlate::carLicense carLicense);
    carLicense FetchFrame();
    bool HasFrame();
};

#endif // LICENSEPLATEUNIT_H
