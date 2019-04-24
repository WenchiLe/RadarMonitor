#include "radarradarcomparethread.h"

RadarRadarCompareThread::RadarRadarCompareThread(RadarUnitData *A,RadarUnitData *B)
{
    radarUnitB = *A;
    radarUnitB = *B;
}

void RadarRadarCompareThread::run()
{

}

float RadarRadarCompareThread::FrameCompare(RadarUnitData::CarInfo carA, RadarUnitData::CarInfo carB, int64_t deltaTime, float deltaLong, float deltaLat)
{
    float diff;
    float x = carA.latitude + carA.latVelocity * deltaTime - deltaLat;
    float y = carA.longtitude + carA.longVelocity * deltaTime - deltaLong;
    diff = qSqrt(qPow(carB.latitude-x,2)+qPow(carB.longtitude-y,2));
    if(diff<THRESHOLD)
    {
        return diff;
    }else{
        return THRESHOLD;
    }
}
