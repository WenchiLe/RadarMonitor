#include "licenseplateunit.h"

static QMutex mutex;

LicensePlateUnit::LicensePlateUnit()
{

}

void LicensePlateUnit::PushNewFrame(ReceiveLicensePlate::carLicense carLicense)
{
    LicensePlateUnit::carLicense mycar;
    mycar.time = carLicense.timeStamp;
    QString s(carLicense.license);
    mycar.license = s;
    mycar.latitude = carLicense.latitude;
    mycar.longtitude = carLicense.longtitude;
    mycar.speed = 0;
    mutex.lock();
    queue_carLicense.enqueue(mycar);
    mutex.unlock();
}

LicensePlateUnit::carLicense LicensePlateUnit::FetchFrame()
{
    LicensePlateUnit::carLicense mycar;
    mycar.time = 0;
    mutex.lock();
    if(!queue_carLicense.isEmpty())
    {
        mycar = queue_carLicense.dequeue();
    }
    mutex.unlock();
    return mycar;
}

bool LicensePlateUnit::HasFrame()
{
    bool hasFrame = false;
    mutex.lock();
    if(!queue_carLicense.isEmpty())
    {
        hasFrame = true;
    }
    mutex.unlock();
    return hasFrame;
}
