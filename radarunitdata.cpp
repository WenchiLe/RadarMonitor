#include "radarunitdata.h"

static QMutex mutex;

RadarUnitData::RadarUnitData()
{

}

void RadarUnitData::SetRadarLocation(QPoint point_radar_location)
{
    this->point_radar_location = point_radar_location;
}

void RadarUnitData::PushNewFrame(ReceiveData::Frame60Bs frame60Bs)
{
    RadarUnitData::Frame frame;
    frame.time = frame60Bs.time;
    frame.car_num = frame60Bs.length;
    for(int i = 0; i < frame60Bs.length; i++)
    {
        RadarUnitData::CarInfo carInfo;
        carInfo.objID = frame60Bs.frame[i][0];
        carInfo.longtitude = frame60Bs.frame[i][1];
        carInfo.latitude = frame60Bs.frame[i][2];
        carInfo.longVelocity = frame60Bs.frame[i][3];
        carInfo.latVelocity = frame60Bs.frame[i][4];
        frame.cars.push_back(carInfo);
    }
    mutex.lock();
    if(queue_frame.count() == MAX_FRAMES_COUNT)
    {
        queue_frame.pop_front();
    }
    queue_frame.push_back(frame);
    mutex.unlock();
}

RadarUnitData::Frame RadarUnitData::FetchFrame()
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    mutex.lock();
    if(!queue_frame.isEmpty()){
        frame = queue_frame[0];
        queue_frame.pop_front();
    }
    mutex.unlock();
    return frame;
}

RadarUnitData::Frame RadarUnitData::GetViewOfFrame(int i)
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    mutex.lock();
    if(queue_frame.count()>i){
        frame = queue_frame[i];
        queue_frame.pop_front();
    }
    mutex.unlock();
    return frame;
}

void RadarUnitData::SetRadarID(int radar_ID)
{
    this->radar_ID = radar_ID;
}

int RadarUnitData::GetFrameCount()
{
    int frameCount;
    mutex.lock();
    frameCount = queue_frame.count();
    mutex.unlock();
    return frameCount;
}

void RadarUnitData::UpdateMapLicense(int objID,QString license)
{
    mutex.lock();
    map_license.insert(objID,license);
    mutex.unlock();
}

QString RadarUnitData::GetLicense(int objID)
{
    QString license = "";
    mutex.lock();
    license = map_license.value(objID,"");
    mutex.unlock();
    return license;
}
