#include "getlicenseplatethread.h"

GetLicensePlateThread::GetLicensePlateThread(ReceiveDataFromServer *receiveDataFromServer)
{
    qRegisterMetaType<ReceiveDataFromServer::CarLicense>("ReceiveDataFromServer::CarLicense");
    //qRegisterMetaType<ReceiveLicensePlate::carLicense>("ReceiveLicensePlate::carLicense");
    //receiveLicensePlate.StartReceiveData();
    flag = true;
    this->receiveDataFromServer = receiveDataFromServer;
}

void GetLicensePlateThread::run()
{
    while(flag)
    {
        if (receiveDataFromServer->HasCarLicense())
        {
            ReceiveDataFromServer::CarLicense carLicense;
            carLicense = receiveDataFromServer->GetQueueCarLicense();
            emit LicensePlateChanged(carLicense);
        }
        //msleep(1000);
    }
}

void GetLicensePlateThread::Stop()
{
    flag = false;
    if(!this->wait(5000))
    {
        qWarning("GetFramesThread : Thread deadlock detected");
        this->terminate();
        this->wait();
    }
}
