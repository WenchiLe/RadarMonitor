#ifndef RADARFRAMEPROCESSTHREAD_H
#define RADARFRAMEPROCESSTHREAD_H

#define RADAR_NUMS 3
#define LAST_RADAR_OVERLAP_ZONE 200
#define THIS_RADAR_OVERLAP_ZONE 50
#define THRESHOLD 50

#include <QObject>
//#include <QThread>
#include <radarunitdata.h>
#include <licenseplateunit.h>
#include <QVector>
#include <QtMath>
#include <iostream>
#include <radarradarcomparethread.h>
#include <licenseradarcomparethread.h>

class RadarFrameProcessThread: public QObject
{
    Q_OBJECT

public:
    RadarFrameProcessThread();
    QString GetLicense(int radarID, int objID);
    void Start();
    void Stop();

private:
    QVector<RadarUnitData> vector_RadarUnitData;
    LicensePlateUnit licensePlateUnit;
    QVector<RadarRadarCompareThread*> vector_RadarRadarCompareThread;
    LicenseRadarCompareThread *licenseRadarCompareThread;

private slots:
    void StoreNewFrames(ReceiveData::Frame60Bs frame60Bs);
    void StoreLicensePlate(ReceiveLicensePlate::carLicense carLicense);
};

#endif // RADARFRAMEPROCESSTHREAD_H
