#include "getframesthread.h"

GetFramesThread::GetFramesThread()
{
    qRegisterMetaType<ReceiveData::Frame60Bs>("ReceiveData::Frame60Bs");
    radar_ID = 0;
    receiveData.StartReceiveData();
}



void GetFramesThread::run()
{
    while(true)
    {
        if (receiveData.HasData())
        {
            ReceiveData::Frame60Bs frame60Bs;
            frame60Bs = receiveData.GetQueue();
            emit ToStoreFrames(frame60Bs);
            //std::cout<<frame60Bs.radar_ID<<std::endl;
            if(frame60Bs.radar_ID == radar_ID)
            {
                emit FramesChanged(frame60Bs);
            }
        }
        //msleep(1000);
    }
}

void GetFramesThread::SetRadarID(int radar_ID)
{
    this->radar_ID = radar_ID;
}
