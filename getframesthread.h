#ifndef GETFRAMESTHREAD_H
#define GETFRAMESTHREAD_H

#include <QObject>
#include <QThread>
#include <receivedata.h>
#include <receivedatafromserver.h>
#include <QVector>
#include <QList>

class GetFramesThread : public QThread
{
     Q_OBJECT

public:
    GetFramesThread();
    void SetRadarID(int radar_ID);
    void Stop();

signals:
void FramesChanged(ReceiveData::Frame60Bs frame60Bs);//signal to draw the last frame
void ToStoreFrames(ReceiveData::Frame60Bs frame60Bs);//signal to store the last frame

protected:
    void run();

private:
    ReceiveData receiveData;
    int radar_ID;
    bool flag;
    ReceiveDataFromServer receiveDataFromServer;
};

#endif // GETFRAMESTHREAD_H
