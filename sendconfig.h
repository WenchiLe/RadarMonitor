#ifndef SENDCONFIG_H
#define SENDCONFIG_H

#define WIN32_LEAN_AND_MEAN

#include <QThread>
#include <qvector.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "2046"

class SendConfig : public QThread
{
    Q_OBJECT

public:
    struct ScaleSetInfo
    {
        int index;
        float max;
        float min;
    };

    QVector<SendConfig::ScaleSetInfo> ScaleSetInfoS;

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

public:
    SendConfig();

protected:
    void run(); //声明继承于QThread虚函数 run()

private slots:
    void GetConfig(QVector<SendConfig::ScaleSetInfo> ScaleSetInfoS);

signals:
    void SendConfigMsg(bool flag);

};

#endif // SENDCONFIG_H
