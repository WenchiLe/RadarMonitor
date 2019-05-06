#include "licenseradarcomparethread.h"

LicenseRadarCompareThread::LicenseRadarCompareThread(LicensePlateUnit *licensePlateUnit,RadarUnitData *radarUnit)
{
    radar = radarUnit;
    licensePlate = licensePlateUnit;
    flag = true;
}

float LicenseRadarCompareThread::LicenseFrameCompare(RadarUnitData::CarInfo car,LicensePlateUnit::carLicense carLicense,int64_t deltaTime)
{
    float diff,diff_dis,diff_speed;
    float x = car.latitude + car.latVelocity * deltaTime/1000;
    float y = car.longtitude + car.longVelocity * deltaTime/1000;
    diff_dis = qSqrt(qPow(carLicense.latitude-x,2)+qPow(carLicense.longtitude-y,2));
    diff_speed = carLicense.speed - qSqrt(qPow(car.latVelocity,2)+qPow(car.longVelocity,2));
    diff = diff_dis + qAbs(diff_speed)*FACTOR;
    //std::cout<<diff<<std::endl;
    if(diff<THRESHOLD)
    {
        return diff;
    }else{
        return THRESHOLD;
    }
}

void LicenseRadarCompareThread::run()
{
    while(flag){
        //wait until there is a license
        while(!licensePlate->HasFrame()&&flag)
        {
            msleep(15);
        }

        //std::cout<<"has data"<<std::endl;
        //fetch a frame from LicensePlate
        LicensePlateUnit::carLicense carLicense = licensePlate->FetchFrame();

        //std::cout<<carLicense.license.toStdString()<<" "<<carLicense.latitude<<" "<<carLicense.longtitude<<std::endl;

        //to insure that there are 2 frames at least from last compared position in radar
        while(!radar->CanCompare()&&flag)
        {
            msleep(15);
        }

        //std::cout<<"can compare"<<std::endl;

        //get compare frame0 from radar
        RadarUnitData::Frame frame0 = radar->GetCompareFrame0();

        //std::cout<<"get frame0"<<std::endl;

        //if carLicense.time is ealier than any frame0.time, then drop this carLicense and get next one
        while(frame0.time > carLicense.time&&flag)
        {
            while(!licensePlate->HasFrame()&&flag)
            {
                msleep(15);
            }
            carLicense = licensePlate->FetchFrame();
            std::cout<<"throw a license"<<std::endl;
        }

        //get compare frame1 from radar
        RadarUnitData::Frame frame1 = radar->GetCompareFrame1();

        //std::cout<<"get frame1"<<std::endl;

        //if carLicense.time is later than any frame1.time, then wait until one frame1 whose time is later than carLicense's
        while(frame1.time <= carLicense.time&&flag)
        {
            if(frame0.used)
            {
                radar->FetchFrame();
            }
            frame0 = frame1;
            while(!radar->CanCompare()&&flag)
            {
                msleep(15);
            }
            radar->UpCompareIndex();
            frame1 = radar->GetCompareFrame1();
        }

        //compare carLicense, frame0, frame1
        double min_threshold_long = carLicense.longtitude-5;
        double max_threshold_long = carLicense.longtitude+5;
        double min_threshold_lat = carLicense.latitude - 5;
        double max_threshold_lat = carLicense.latitude + 5;
        int64_t deltaTime_F0_Lic = frame0.time - carLicense.time;
        int64_t deltaTime_F1_Lic = frame1.time - carLicense.time;
        float diff = THRESHOLD;
        float mindiff = THRESHOLD;
        int objID = -1;
        foreach(RadarUnitData::CarInfo car0, frame0.cars)
        {
            if(car0.longtitude > max_threshold_long ||car0.longtitude < min_threshold_long||car0.latitude>max_threshold_lat||car0.latitude<min_threshold_lat)
            {
                continue;
            }
            diff = LicenseFrameCompare(car0,carLicense,deltaTime_F0_Lic);
            if(diff<mindiff)
            {
                mindiff = diff;
                objID = car0.objID;
            }
        }
        foreach(RadarUnitData::CarInfo car1, frame1.cars)
        {
            if(car1.longtitude > max_threshold_long ||car1.longtitude < min_threshold_long||car1.latitude>max_threshold_lat||car1.latitude<min_threshold_lat)
            {
                continue;
            }
            diff = LicenseFrameCompare(car1,carLicense,deltaTime_F1_Lic);
            if(diff<mindiff)
            {
                mindiff = diff;
                objID = car1.objID;
            }
        }

        //update the map_license
        //std::cout<<"license and radar #0 has matched: objID: "<<objID<<" license: "<<carLicense.license.toStdString()<<std::endl;
        radar->UpdateMapLicense(objID,carLicense.license);
    }
}

void LicenseRadarCompareThread::Stop()
{
    flag = false;
    if(!this->wait(5000))
    {
        qWarning("LicenseRadarCompareThread : Thread deadlock detected");
        this->terminate();
        this->wait();
    }
}
