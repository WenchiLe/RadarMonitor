#include "radarunitdata.h"

static QMutex mutex;

RadarUnitData::RadarUnitData()
{

}

void RadarUnitData::SetRadarLocation(QPoint point_radar_location)
{
    this->point_radar_location = point_radar_location;
}

void RadarUnitData::PushNewFrame(FrameStructData frame60Bs)
{
    RadarUnitData::Frame frame;
    frame.time = frame60Bs.timeStamp;
    frame.car_num = frame60Bs.length;
    frame.used = false;
    for (int i = 0; i < frame60Bs.length; i++)
    {
    RadarUnitData::CarInfo carInfo;
    carInfo.objID = frame60Bs.frameData[i].objId;
    carInfo.longtitude = frame60Bs.frameData[i].currInfo.distLong;
    carInfo.latitude = frame60Bs.frameData[i].currInfo.distLat;
    carInfo.longVelocity = frame60Bs.frameData[i].currInfo.velLong;
    carInfo.latVelocity = frame60Bs.frameData[i].currInfo.velLat;
    frame.cars.push_back(carInfo);
    }
    mutex.lock();
    if (queue_frame.count() == MAX_FRAMES_COUNT)
    {
    queue_frame.pop_front();
    if (index > 0)
    {
        index--;
    }
    if (indexCompare0 > 0)
    {
        indexCompare0--;
    }
    if (indexCompare1 > 1)
    {
        indexCompare1--;
    }
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
    if (!queue_frame.isEmpty())
    {
    frame = queue_frame[0];
    queue_frame.pop_front();
    if (index > 0)
    {
        index--;
    }
    if (indexCompare0 > 0)
    {
        indexCompare0--;
    }
    if (indexCompare1 > 1)
    {
        indexCompare1--;
    }
    }
    mutex.unlock();
    return frame;
}

RadarUnitData::Frame RadarUnitData::GetViewOfFrame(int i)
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    frame.used = true;
    mutex.lock();
    if (queue_frame.count() > i)
    {
    frame = queue_frame[i];
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

void RadarUnitData::UpdateMapLicense(int objID, QString license)
{
    mutex.lock();
    map_license.insert(objID, license);
    mutex.unlock();
}

QString RadarUnitData::GetLicense(int objID)
{
    QString license = "";
    mutex.lock();
    license = map_license.value(objID, "");
    mutex.unlock();
    return license;
}

RadarUnitData::Frame RadarUnitData::GetNextProcessFrame()
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    frame.used = true;
    mutex.lock();
    if (queue_frame.count() > index)
    {
    frame = queue_frame[index];
    queue_frame[index].used = true;
    if (index < queue_frame.count() - 1)
    {
        index++;
    }
    }
    mutex.unlock();
    return frame;
}

RadarUnitData::Frame RadarUnitData::GetCompareFrame0()
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    frame.used = false;
    mutex.lock();
    if (queue_frame.count() > indexCompare0)
    {
    frame = queue_frame[indexCompare0];
    }
    mutex.unlock();
    return frame;
}

RadarUnitData::Frame RadarUnitData::GetCompareFrame1()
{
    RadarUnitData::Frame frame;
    frame.car_num = 0;
    frame.time = 0;
    frame.used = false;
    mutex.lock();
    if (queue_frame.count() > indexCompare1)
    {
    frame = queue_frame[indexCompare1];
    }
    mutex.unlock();
    return frame;
}

void RadarUnitData::UpCompareIndex()
{
    mutex.lock();
    if (queue_frame.count() - 1 > indexCompare1)
    {
    indexCompare1++;
    indexCompare0++;
    }
    mutex.unlock();
}

bool RadarUnitData::CanCompare()
{
    bool flag = false;
    mutex.lock();
    if (queue_frame.count() - indexCompare0 >= 2)
    {
    flag = true;
    }
    mutex.unlock();
    return flag;
}
