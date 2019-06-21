#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

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
#define DEFAULT_PORT "2047"

class ReceiveData : public QThread
{
    Q_OBJECT
public:
    struct Frame60Bs
    {
    int radar_ID;//the index of radar
    int length;//the number of objects
    int64_t time;//the time of frame;
    float frame[256][6];//[0]:object_ID; [1]:dis_long; [2]:dis_lat; [3]:V_long; [4]:V_lat [5]:state;
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
    QQueue<Frame60Bs> queueFrame60Bs;

public:
    ReceiveData();
    void StartReceiveData();
    void StopReceiveData();
    Frame60Bs GetQueue();
    bool HasData();

protected:
    void run();
};

#endif // RECEIVEDATA_H
