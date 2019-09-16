#ifndef RADARUNITDATA_H
#define RADARUNITDATA_H

#define MAX_FRAMES_COUNT 1024

#include <QMutex>
#include <QtCore>
#include <QQueue>
#include <QMap>
#include <QVector>
#include <receivedata.h>
#include <QReadWriteLock>
#include <QList>

class RadarUnitData
{
public:
    int radar_ID;
    QPoint point_radar_location;

    QMap<int, QString> map_license;

    struct UnitCompare
    {
    int objID_this_radar;
    int objID_last_radar;
    float diff;
    QString license;
    };
    QList<UnitCompare> CompareState;

    struct CarInfo
    {
    int objID;
    int state;
    float longtitude;
    float latitude;
    float longVelocity;
    float latVelocity;
    };
    struct Frame
    {
    int64_t time;
    int car_num;
    QVector<CarInfo> cars;
    bool used;
    };
    QVector<Frame> queue_frame;

private:
    int index = 0;
    int indexCompare0 = 0;
    int indexCompare1 = 1;

public:
    RadarUnitData();
    void SetRadarLocation(QPoint point_radar_location);
    void PushNewFrame(FrameStructData frame60Bs);
    Frame FetchFrame();
    Frame GetViewOfFrame(int i);
    void SetRadarID(int radar_ID);
    int GetFrameCount();
    void UpdateMapLicense(int objID, QString license);
    QString GetLicense(int objID);
    Frame GetNextProcessFrame();
    Frame GetCompareFrame0();
    Frame GetCompareFrame1();
    void UpCompareIndex();
    bool CanCompare();
};

#endif // RADARUNITDATA_H
