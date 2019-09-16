#include "getframesthread.h"

GetFramesThread::GetFramesThread()
{
    qRegisterMetaType<FrameStructData>("FrameStructData");
    radar_ID = 0;
    receiveData.StartReceiveData();
    flag = true;
}



void GetFramesThread::run()
{
    while (flag)
    {
    if (receiveData.HasData())
    {
        //std::cout << "hasdata" << std::endl;
        FrameStructData frame60Bs;
        frame60Bs = receiveData.GetQueue();
        emit ToStoreFrames(frame60Bs);
        //std::cout<<frame60Bs.radar_ID<<std::endl;
        if (frame60Bs.radarID - 1 == radar_ID)
        {
        emit FramesChanged(frame60Bs);
        }
    }
    //msleep(1000);
    }
}

void GetFramesThread::Stop()
{
    flag = false;
    if (!this->wait(5000))
    {
    qWarning("GetFramesThread : Thread deadlock detected");
    this->terminate();
    this->wait();
    }
    receiveData.StopReceiveData();
}

void GetFramesThread::SetRadarID(int radar_ID)
{
    this->radar_ID = radar_ID;
}
