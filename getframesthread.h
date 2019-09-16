#ifndef GETFRAMESTHREAD_H
#define GETFRAMESTHREAD_H

#define RADAR_NUM 3

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
    GetFramesThread(ReceiveDataFromServer *receiveDataFromServer);
    void SetRadarID(int radar_ID);
    void Stop();
    void SetRadarAngle(qreal angle);

signals:
    void FramesChanged(FrameStructData frame60Bs);//signal to draw the last frame
    void ToStoreFrames(FrameStructData frame60Bs);//signal to store the last frame

protected:
    void run();

private:
    //ReceiveData receiveData;
    int radar_ID;
    bool flag;
    ReceiveDataFromServer *receiveDataFromServer;
    qreal radarAngle[RADAR_NUM] = {0};
};

#endif // GETFRAMESTHREAD_H
