#include "getlicenseplatethread.h"

GetLicensePlateThread::GetLicensePlateThread()
{
    qRegisterMetaType<ReceiveLicensePlate::carLicense>("ReceiveLicensePlate::carLicense");
    receiveLicensePlate.StartReceiveData();
}

void GetLicensePlateThread::run()
{
    while(true)
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
