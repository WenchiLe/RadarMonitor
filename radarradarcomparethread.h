#ifndef RADARRADARCOMPARETHREAD_H
#define RADARRADARCOMPARETHREAD_H

#define LAST_RADAR_OVERLAP_ZONE 200
#define THIS_RADAR_OVERLAP_ZONE 50
#define THRESHOLD 20
#define FACTOR 3

#include <QObject>
#include <QThread>
#include <radarunitdata.h>
#include <QtMath>
#include <iostream>

class RadarRadarCompareThread : public QThread
{
    Q_OBJECT

public:
    RadarRadarCompareThread(RadarUnitData *A,RadarUnitData *B);
    void Stop();

private:
    float FrameCompare(RadarUnitData::CarInfo carA, RadarUnitData::CarInfo carB, int64_t deltaTime, float deltaLong, float deltaLat);

protected:
    void run();

private:
    //compare RadarUnitData A and B, and transmit the license in A to B
    RadarUnitData *radarUnitA;
    RadarUnitData *radarUnitB;
    bool flag;
};

#endif // RADARRADARCOMPARETHREAD_H
