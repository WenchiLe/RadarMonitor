#include "radarframeprocessthread.h"

RadarFrameProcessThread::RadarFrameProcessThread()
{
    for (int i = 0; i < RADAR_NUMS; i++)
    {
    RadarUnitData radarUnitData;
    radarUnitData.SetRadarID(i);
    if (i == 0)
    {
        for (int j = 0; j < 255; j++)
        {
        qsrand(j);
        QString carMark = QString("%1").arg(qrand() % 300, 3, 10, QChar('0'));
        radarUnitData.UpdateMapLicense(j, carMark);
        }
    }
    radarUnitData.SetRadarLocation(QPoint(0, 140 * i));
    vector_RadarUnitData.push_back(radarUnitData);
    }
    licenseRadarCompareThread = new LicenseRadarCompareThread(&licensePlateUnit, &vector_RadarUnitData[0]);
    for (int i = 0; i < RADAR_NUMS - 1; i++)
    {
    RadarRadarCompareThread *radarRadarCompareThread = new RadarRadarCompareThread(&vector_RadarUnitData[i], &vector_RadarUnitData[i + 1]);
    vector_RadarRadarCompareThread.push_back(radarRadarCompareThread);
    }
}

void RadarFrameProcessThread::Start()
{
    licenseRadarCompareThread->start();
    for (int i = 0; i < RADAR_NUMS - 1; i++)
    {
    vector_RadarRadarCompareThread[i]->start();
    }
}

void RadarFrameProcessThread::Stop()
{
    licenseRadarCompareThread->Stop();
    delete licenseRadarCompareThread;
    for (int i = 0; i < RADAR_NUMS - 1; i++)
    {
    vector_RadarRadarCompareThread[i]->Stop();
    delete vector_RadarRadarCompareThread[i];
    }
}

void RadarFrameProcessThread::StoreNewFrames(ReceiveDataFromServer::Frame60Bs frame60Bs)
{
    if (frame60Bs.radarId >= 1 && frame60Bs.radarId <= RADAR_NUMS)
    {
    vector_RadarUnitData[frame60Bs.radarId - 1].PushNewFrame(frame60Bs);
    }
}

void RadarFrameProcessThread::StoreLicensePlate(ReceiveDataFromServer::CarLicense carLicense)
{
    licensePlateUnit.PushNewFrame(carLicense);
}

QString RadarFrameProcessThread::GetLicense(int radarID, int objID)
{
    return vector_RadarUnitData[radarID].GetLicense(objID);
}
