#ifndef LICENSERADARCOMPARETHREAD_H
#define LICENSERADARCOMPARETHREAD_H

#define THRESHOLD 10
#define FACTOR 0

#include <QObject>
#include <QThread>
#include <radarunitdata.h>
#include <licenseplateunit.h>
#include <QtMath>
#include <iostream>

class LicenseRadarCompareThread : public QThread
{
    Q_OBJECT

public:
    LicenseRadarCompareThread(LicensePlateUnit *licensePlateUnit,RadarUnitData *radarUnit);
    float LicenseFrameCompare(RadarUnitData::CarInfo car,LicensePlateUnit::carLicense carLicense,int64_t deltaTime);
    void Stop();

protected:
    void run();

private:
    //compare Radar and LicensePlate, and transmit the license to radar
    RadarUnitData *radar;
    LicensePlateUnit *licensePlate;
    bool flag;
};

#endif // LICENSERADARCOMPARETHREAD_H
