#include "getlicenseplatethread.h"

GetLicensePlateThread::GetLicensePlateThread()
{
    qRegisterMetaType<ReceiveLicensePlate::carLicense>("ReceiveLicensePlate::carLicense");
    receiveLicensePlate.StartReceiveData();
    flag = true;
}

void GetLicensePlateThread::run()
{
    while(flag)
    {
        if (receiveLicensePlate.HasData())
        {
            ReceiveLicensePlate::carLicense frame60Bs;
            frame60Bs = receiveLicensePlate.GetQueue();
            emit LicensePlateChanged(frame60Bs);
        }
        //msleep(1000);
    }
}

void GetLicensePlateThread::Stop()
{
    receiveLicensePlate.StopReceiveData();
    flag = false;
    if(!this->wait(5000))
    {
        qWarning("GetFramesThread : Thread deadlock detected");
        this->terminate();
        this->wait();
    }
}
