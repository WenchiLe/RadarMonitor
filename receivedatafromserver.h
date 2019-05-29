#ifndef RECEIVEDATAFROMSERVER_H
#define RECEIVEDATAFROMSERVER_H

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#elif defined(__GNUC__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#error Unsupported platform
#endif

#define DEFAULT_PORT 2046

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QtNetwork>

class ReceiveDataFromServer : public QThread
{
    Q_OBJECT
public:
    typedef PACK(struct
    {
    int32_t number;
    int32_t packetLength;
    }) MagicHeader;

    typedef PACK(struct
    {
    int32_t clientId;
    }) LiveView;

    typedef PACK(struct
    {
    int32_t nodeId;
    double speed;
    double longitude;
    double latitude;
    int64_t timeStamp;
    char license[20];
    }) CarLicense;

    typedef PACK(struct
    {
    int32_t radarId;
    int32_t length;
    int64_t timeStamp;
    float frameData[256][6];
    }) Frame60Bs;

    int ListenFlag = 0;

    QMutex mutexFrame60Bs;
    QMutex mutexCarLicense;
    QQueue<Frame60Bs> queueFrame60Bs;
    QQueue<CarLicense> queueCarLicense;

    QTcpSocket *client;
    QDataStream in;
    QString readHeadOrBody;
    MagicHeader magicHeader;

public:
    ReceiveDataFromServer();
    void StartReceiveData();
    void StopReceiveData();
    void CarLicenseHandler(const char *buffer);
    void Frame60BsHandler(const char *buffer);
    Frame60Bs GetQueueFrame60Bs();
    bool HasFrame60Bs();
    CarLicense GetQueueCarLicense();
    bool HasCarLicense();

protected:
    void run();

private slots:
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
};

#endif // RECEIVEDATAFROMSERVER_H
