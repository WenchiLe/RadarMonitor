#include "getframesthread.h"

GetFramesThread::GetFramesThread(ReceiveDataFromServer *receiveDataFromServer)
{
    //qRegisterMetaType<ReceiveData::Frame60Bs>("ReceiveData::Frame60Bs");
    qRegisterMetaType<ReceiveDataFromServer::Frame60Bs>("ReceiveDataFromServer::Frame60Bs");
    radar_ID = 0;
    //receiveData.StartReceiveData();
    //receiveDataFromServer.StartReceiveData();
    flag = true;
    this->receiveDataFromServer = receiveDataFromServer;
}



void GetFramesThread::run()
{
    while (flag)
    {
    if (receiveDataFromServer->HasFrame60Bs())
    {
        ReceiveDataFromServer::Frame60Bs frame60Bs;
        frame60Bs = receiveDataFromServer->GetQueueFrame60Bs();
        //todo : process the angle
        qreal angle = qDegreesToRadians(radarAngle[frame60Bs.radarId - 1]);
        for (int j = 0; j < frame60Bs.length; j++)
        {
        float x = frame60Bs.frameData[j][2];
        float y = frame60Bs.frameData[j][1];
        frame60Bs.frameData[j][2] = x * qCos(angle) - y * qSin(angle);
        frame60Bs.frameData[j][1] = y * qCos(angle) + x * qSin(angle);
        }

        emit ToStoreFrames(frame60Bs);
        //std::cout<<frame60Bs.radar_ID<<std::endl;
        if (frame60Bs.radarId - 1 == radar_ID)
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
}

void GetFramesThread::SetRadarID(int radar_ID)
{
    this->radar_ID = radar_ID;
}

void GetFramesThread::SetRadarAngle(qreal angle)
{
    radarAngle[radar_ID] += angle;
    std::cout << radar_ID << " : " << radarAngle[radar_ID] << std::endl;
}
