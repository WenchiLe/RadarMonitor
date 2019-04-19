#ifndef RADARFRAMEPROCESSTHREAD_H
#define RADARFRAMEPROCESSTHREAD_H

#define RADAR_NUMS 4
#define LAST_RADAR_OVERLAP_ZONE 150
#define THIS_RADAR_OVERLAP_ZONE 50
#define THRESHOLD 50

#include <QObject>
#include <QThread>
#include <radarunitdata.h>
#include <receivelicenseplate.h>
#include <licenseplateunit.h>
#include <QVector>
#include <QtMath>

class RadarFrameProcessThread : public QThread
{
    Q_OBJECT

public:
    RadarFrameProcessThread();
    QString GetLicense(int radarID, int objID);

private:
    float FrameCompare(RadarUnitData::CarInfo carA, RadarUnitData::CarInfo carB, int64_t deltaTime, float deltaLong, float deltaLat);
    float LicenseFrameCompare(RadarUnitData::CarInfo car,LicensePlateUnit::carLicense carLicense,int64_t deltaTime);

protected:
    void run();

private:
    QVector<RadarUnitData> vector_RadarUnitData;
    LicensePlateUnit licensePlateUnit;

private slots:
    void StoreNewFrames(ReceiveData::Frame60Bs frame60Bs);
    void StoreLicensePlate(ReceiveLicensePlate::carLicense carLicense);
};

#endif // RADARFRAMEPROCESSTHREAD_H
