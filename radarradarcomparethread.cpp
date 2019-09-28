#include "radarradarcomparethread.h"

RadarRadarCompareThread::RadarRadarCompareThread(RadarUnitData *A, RadarUnitData *B)
{
    radarUnitA = A;
    radarUnitB = B;
    flag = true;
}

void RadarRadarCompareThread::run()
{
    //initial: to insure A has data
    //std::cout<<"radar #"<<radarUnitA->radar_ID+1<<" and radar #"<<radarUnitB->radar_ID+1<<" compare thread start"<<std::endl;
    while (radarUnitA->GetFrameCount() == 0 && flag)
    {
    msleep(15);
    //std::cout<<"radar #"<<(*radarUnitA).radar_ID+1<<"has no data"<<std::endl;
    }

    //std::cout<<"radar #"<<radarUnitA->radar_ID+1<<"has data"<<std::endl;

    while (flag)
    {
    //get a new unused frameA from A
    RadarUnitData::Frame frameA = radarUnitA->GetNextProcessFrame();
    while (frameA.used && flag)
    {
        msleep(15);
        frameA = radarUnitA->GetNextProcessFrame();
    }

    //to insure that there are 2 frames at least from last compared position in B
    while (!radarUnitB->CanCompare() && flag)
    {
        msleep(15);
    }

    //get compare frameB0 from B
    RadarUnitData::Frame frameB0 = radarUnitB->GetCompareFrame0();

    //if frameA.time is ealier than any frameB.time, then drop this frameA and get next unused frameA
    while (frameB0.time > frameA.time && (!frameA.used) && flag)
    {
        msleep(15);
        frameA = radarUnitA->GetNextProcessFrame();
    }

    //get compare frameB1 from B
    RadarUnitData::Frame frameB1 = radarUnitB->GetCompareFrame1();

    //if frameA.time is later than any frameB.time, then wait until one frameB is later than frameA
    while (frameB1.time <= frameA.time && flag)
    {
        if (frameB0.used)
        {
        radarUnitB->FetchFrame();
        }
        frameB0 = frameB1;
        while (!radarUnitB->CanCompare())
        {
        msleep(15);
        }
        radarUnitB->UpCompareIndex();
        frameB1 = radarUnitB->GetCompareFrame1();
    }

    //compare frameA, frameB0, frameB1
    int64_t deltaTime_B0_A = frameB0.time - frameA.time;
    int64_t deltaTime_B1_A = frameB1.time - frameA.time;
    float deltaLong = radarUnitB->point_radar_location.y() - radarUnitA->point_radar_location.y();
    float deltaLat = radarUnitB->point_radar_location.x() - radarUnitA->point_radar_location.x();
    radarUnitB->CompareState.clear();
    foreach (RadarUnitData::CarInfo carA, frameA.cars)
    {
        if (carA.longtitude < deltaLong)
        {
        continue;
        }
        float diff = THRESHOLD;
        float minDiff = THRESHOLD;
        int objID_B = -1;
        foreach (RadarUnitData::CarInfo carB0, frameB0.cars)
        {
        if (carB0.longtitude > THIS_RADAR_OVERLAP_ZONE)
        {
            continue;
        }
        diff = FrameCompare(carA, carB0, deltaTime_B0_A, deltaLong, deltaLat);
        if (diff < minDiff)
        {
            minDiff = diff;
            objID_B = carB0.objID;
        }
        }
        foreach (RadarUnitData::CarInfo carB1, frameB1.cars)
        {
        if (carB1.longtitude > THIS_RADAR_OVERLAP_ZONE)
        {
            continue;
        }
        diff = FrameCompare(carA, carB1, deltaTime_B1_A, deltaLong, deltaLat);
        if (diff < minDiff)
        {
            minDiff = diff;
            objID_B = carB1.objID;
        }
        }

        //push compare result into CompareState List; (if objID_B is repeated, choose whose diff is smaller)
        RadarUnitData:: UnitCompare unitCompare;
        unitCompare.objID_this_radar = objID_B;
        unitCompare.objID_last_radar = carA.objID;
        unitCompare.diff = minDiff;
        bool isRepeated = false;
        foreach (RadarUnitData:: UnitCompare item, (*radarUnitB).CompareState)
        {
        if (item.objID_this_radar == unitCompare.objID_this_radar)
        {
            isRepeated = true;
            if (item.diff > unitCompare.diff)
            {
            item.diff = unitCompare.diff;
            item.objID_last_radar = unitCompare.objID_last_radar;
            }
        }
        }
        if (!isRepeated)
        {
        radarUnitB->CompareState.append(unitCompare);
        }
    }

    //update the map_license
    //std::cout<<"radar #"<<radarUnitA->radar_ID+1<<" and radar #"<<radarUnitB->radar_ID+1<<" has matched:"<<std::endl;
    foreach (RadarUnitData:: UnitCompare item, radarUnitB->CompareState)
    {
        int objID_A = item.objID_last_radar;
        int objID_B = item.objID_this_radar;
        QString license = radarUnitA->GetLicense(objID_A);
        //std::cout<<"objID_A:"<<objID_A<<" objID_B:"<<objID_B<<" license: "<<license.toStdString()<<std::endl;
        radarUnitB->UpdateMapLicense(objID_B, license);
    }
    }
}

float RadarRadarCompareThread::FrameCompare(RadarUnitData::CarInfo carA, RadarUnitData::CarInfo carB, int64_t deltaTime, float deltaLong, float deltaLat)
{
    float diff, diff_dis, diff_speed;
    float x = carA.latitude + carA.latVelocity * deltaTime / 1000 - deltaLat;
    float y = carA.longtitude + carA.longVelocity * deltaTime / 1000 - deltaLong;
    diff_dis = qSqrt(qPow(carB.latitude - x, 2) + qPow(carB.longtitude - y, 2));
    if ((carA.longVelocity * carB.longVelocity) < 0)
    {
    return THRESHOLD;
    }
    diff_speed = qSqrt(qPow(carA.latVelocity, 2) + qPow(carA.longVelocity, 2)) - qSqrt(qPow(carB.latVelocity, 2) + qPow(carB.longVelocity, 2));
    diff = diff_dis;// + qAbs(diff_speed) * FACTOR;
    if (diff < THRESHOLD)
    {
    return diff;
    }
    else
    {
    return THRESHOLD;
    }
}

void RadarRadarCompareThread::Stop()
{
    flag = false;
    if (!this->wait(5000))
    {
    qWarning("RadarRadarCompareThread : Thread deadlock detected");
    this->terminate();
    this->wait();
    }
}
