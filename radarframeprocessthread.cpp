#include "radarframeprocessthread.h"

RadarFrameProcessThread::RadarFrameProcessThread()
{
    for(int i = 0; i < RADAR_NUMS; i++)
    {
        RadarUnitData radarUnitData;
        radarUnitData.SetRadarID(i);
        vector_RadarUnitData.push_back(radarUnitData);
    }
}

void RadarFrameProcessThread::run()
{
    //initial: each radar push 20 frames
    for(int radarID = 0; radarID < RADAR_NUMS; radarID++)
    {
        if(vector_RadarUnitData[radarID].GetFrameCount()<20)
        {
            msleep(15);
            radarID--;
        }
    }
    //pop 19 frames to get the 20th frame of radar#0
    for(int i = 0; i < 19; i++)
    {
        vector_RadarUnitData[0].FetchFrame();
    }

    //compare frames
    while(true)
    {
        //process LicensePlate and the 1st radar:radar #0
        if(licensePlateUnit.HasFrame())
        {
            LicensePlateUnit::carLicense carLicense = licensePlateUnit.FetchFrame();

            //to insure that there are 2 frames in radar #0 at least
            while(vector_RadarUnitData[0].GetFrameCount()<2)
            {
                msleep(15);
            }

            RadarUnitData::Frame frame0 = vector_RadarUnitData[0].GetViewOfFrame(0);

            //if carLicense.time is ealier than any frameRadar0.time, then drop this carLicense and get next carLicense
            while(frame0.time > carLicense.time)
            {
                if(vector_RadarUnitData[0].GetFrameCount() == 0)
                {
                    msleep(15);
                    continue;
                }
                frame0 = vector_RadarUnitData[0].FetchFrame();
            }

            RadarUnitData::Frame frame1 = vector_RadarUnitData[0].GetViewOfFrame(0);

            //if carLicense.time is later than any frameRadar0.time, then wait until one frameRadar0 is later than carLicense
            while(frame1.time <= carLicense.time)
            {
                frame0 = frame1;
                vector_RadarUnitData[0].FetchFrame();
                while(vector_RadarUnitData[0].GetFrameCount()<2)
                {
                    msleep(15);
                }
                frame1 = vector_RadarUnitData[0].GetViewOfFrame(1);
            }

            //compare carLicense, frame0, frame1
            double min_threshold_long = carLicense.longtitude/2;
            double max_threshold_long = carLicense.longtitude*1.5;
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
            vector_RadarUnitData[0].UpdateMapLicense(objID,carLicense.license);
        }

        //process radar
        for(int radarID = 0; radarID < RADAR_NUMS - 1; radarID++)
        {
            RadarUnitData::Frame frameA = vector_RadarUnitData[radarID].FetchFrame();

            //to insure that there are 2 frames in B at least
            while(vector_RadarUnitData[radarID+1].GetFrameCount()<2)
            {
                msleep(15);
            }

            RadarUnitData::Frame frameB0 = vector_RadarUnitData[radarID+1].GetViewOfFrame(0);

            //if frameA.time is ealier than any frameB.time, then drop this frameA and get next frameA
            while(frameB0.time > frameA.time)
            {
                if(vector_RadarUnitData[radarID].GetFrameCount() == 0)
                {
                    msleep(15);
                    continue;
                }
                frameA = vector_RadarUnitData[radarID].FetchFrame();
            }

            RadarUnitData::Frame frameB1 = vector_RadarUnitData[radarID+1].GetViewOfFrame(1);

            //if frameA.time is later than any frameB.time, then wait until one frameB is later than frameA
            while(frameB1.time <= frameA.time)
            {
                frameB0 = frameB1;
                vector_RadarUnitData[radarID+1].FetchFrame();
                while(vector_RadarUnitData[radarID+1].GetFrameCount()<2)
                {
                    msleep(15);
                }
                frameB1 = vector_RadarUnitData[radarID+1].GetViewOfFrame(1);
            }

            //compare frameA, frameB0, frameB1
            int64_t deltaTime_B0_A = frameB0.time - frameA.time;
            int64_t deltaTime_B1_A = frameB1.time - frameA.time;
            float deltaLong = vector_RadarUnitData[radarID+1].point_radar_location.y()-vector_RadarUnitData[radarID].point_radar_location.y();
            float deltaLat = vector_RadarUnitData[radarID+1].point_radar_location.x()-vector_RadarUnitData[radarID].point_radar_location.x();
            foreach (RadarUnitData::CarInfo carA, frameA.cars)
            {
                if(carA.longtitude < LAST_RADAR_OVERLAP_ZONE)
                {
                    continue;
                }
                float diff = THRESHOLD;
                float minDiff = THRESHOLD;
                int objID_B = -1;
                foreach(RadarUnitData::CarInfo carB0, frameA.cars)
                {
                    if(carB0.longtitude>THIS_RADAR_OVERLAP_ZONE)
                    {
                        continue;
                    }
                    diff = FrameCompare(carA,carB0,deltaTime_B0_A,deltaLong,deltaLat);
                    if(diff<minDiff)
                    {
                        minDiff = diff;
                        objID_B = carB0.objID;
                    }
                }
                foreach(RadarUnitData::CarInfo carB1, frameA.cars)
                {
                    if(carB1.longtitude>THIS_RADAR_OVERLAP_ZONE)
                    {
                        continue;
                    }
                    diff = FrameCompare(carA,carB1,deltaTime_B1_A,deltaLong,deltaLat);
                    if(diff<minDiff)
                    {
                        minDiff = diff;
                        objID_B = carB1.objID;
                    }
                }

                //push compare result into CompareState List; (if objID_B is repeated, choose whose diff is smaller)
                vector_RadarUnitData[radarID+1].CompareState.clear();
                RadarUnitData:: UnitCompare unitCompare;
                unitCompare.objID_this_radar = objID_B;
                unitCompare.objID_last_radar = carA.objID;
                unitCompare.diff = minDiff;
                bool isRepeated = false;
                foreach(RadarUnitData:: UnitCompare item,vector_RadarUnitData[radarID+1].CompareState)
                {
                    if(item.objID_this_radar == unitCompare.objID_this_radar)
                    {
                        isRepeated = true;
                        if(item.diff>unitCompare.diff)
                        {
                            item.diff = unitCompare.diff;
                            item.objID_last_radar = unitCompare.objID_last_radar;
                        }
                    }
                }
                if(!isRepeated)
                {
                    vector_RadarUnitData[radarID+1].CompareState.append(unitCompare);
                }
            }

            //update the map_license
            foreach(RadarUnitData:: UnitCompare item,vector_RadarUnitData[radarID+1].CompareState)
            {
                int objID_A = item.objID_last_radar;
                int objID_B = item.objID_this_radar;
                QString license = vector_RadarUnitData[radarID].GetLicense(objID_A);
                vector_RadarUnitData[radarID+1].UpdateMapLicense(objID_B,license);
            }
        }
    }
}

void RadarFrameProcessThread::StoreNewFrames(ReceiveData::Frame60Bs frame60Bs)
{
    vector_RadarUnitData[frame60Bs.radar_ID].PushNewFrame(frame60Bs);
}

float RadarFrameProcessThread::FrameCompare(RadarUnitData::CarInfo carA, RadarUnitData::CarInfo carB, int64_t deltaTime, float deltaLong, float deltaLat)
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

void RadarFrameProcessThread::StoreLicensePlate(ReceiveLicensePlate::carLicense carLicense)
{
    licensePlateUnit.PushNewFrame(carLicense);
}

float RadarFrameProcessThread::LicenseFrameCompare(RadarUnitData::CarInfo car,LicensePlateUnit::carLicense carLicense,int64_t deltaTime)
{
    float diff;
    float x = car.latitude + car.latVelocity * deltaTime;
    float y = car.longtitude + car.longVelocity * deltaTime;
    diff = qSqrt(qPow(carLicense.latitude-x,2)+qPow(carLicense.longtitude-y,2));
    if(diff<THRESHOLD)
    {
        return diff;
    }else{
        return THRESHOLD;
    }
}

QString RadarFrameProcessThread::GetLicense(int radarID, int objID)
{
    return vector_RadarUnitData[radarID].GetLicense(objID);
}
