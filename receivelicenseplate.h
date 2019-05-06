#ifndef RECEIVELICENSEPLATE_H
#define RECEIVELICENSEPLATE_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <QObject>
#include <QThread>
#include <QQueue>
#include <QMutex>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "2045"

class ReceiveLicensePlate : public QThread
{
    Q_OBJECT
public:
    struct carLicense
    {
        //double speed = 0;
        double longtitude = 0.0;
        double latitude = 0.0;
        int64_t timeStamp = 0;
        char license[20];
    };

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int ListenFlag = 0;

    QMutex mutex;
    QQueue<carLicense> queueCarLicense;

public:
    ReceiveLicensePlate();
    void StartReceiveData();
    void StopReceiveData();
    carLicense GetQueue();
    bool HasData();

protected:
    void run();
};

#endif // RECEIVELICENSEPLATE_H
