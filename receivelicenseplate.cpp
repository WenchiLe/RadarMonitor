#include "receivelicenseplate.h"

ReceiveLicensePlate::ReceiveLicensePlate()
{

}

void ReceiveLicensePlate::StartReceiveData()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    ListenFlag = 1;

    this->start();
}

void ReceiveLicensePlate::run()
{
    while (ListenFlag == 1)
    {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        //Receive until the peer shuts down the connection
        do
        {
            carLicense frame60Bs;
            iResult = recv(ClientSocket, (char*)&frame60Bs, sizeof(frame60Bs), 0);
            if (iResult > 0) {
                //printf("Bytes received: %d\n", iResult);
                if (iResult == sizeof(carLicense))
                {
                    //cout << "data.length:" << frame60Bs.length << endl;
                    //for (int j = 0;j < frame60Bs.length;j++)
                    //{
                    //    cout << "ObjID:" << frame60Bs.frame[j][0] << endl;
                    //    cout << "ObjDistLong:" << frame60Bs.frame[j][1] << endl;
                    //    cout << "ObjDistLat:" << frame60Bs.frame[j][2] << endl;
                    //    cout << "ObjVelocityLong:" << frame60Bs.frame[j][3] << endl;
                    //    cout << "ObjvelocityLat:" << frame60Bs.frame[j][4] << endl;
                    //}
                    mutex.lock();
                    queueCarLicense.enqueue(frame60Bs);
                    mutex.unlock();
                }
                else {
                    std::cout << "package lost" << std::endl;
                }
            }
            else if (iResult == 0)
                printf("no data...\n");
            else {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
            }
        } while (iResult > 0);

        iResult = shutdown(ClientSocket, SD_RECEIVE);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
        }
        closesocket(ClientSocket);
    }
}

void ReceiveLicensePlate::StopReceiveData()
{
    ListenFlag = 0;
    if(!this->wait(5000))
    {
        qWarning("ReceiveData : Thread deadlock detected");
        this->terminate();
        this->wait();
    }

    iResult = shutdown(ClientSocket, SD_RECEIVE);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
    }
    closesocket(ClientSocket);
    // cleanup
    closesocket(ListenSocket);

    WSACleanup();
}


ReceiveLicensePlate::carLicense ReceiveLicensePlate::GetQueue()
{
    carLicense frame60Bs;
    mutex.lock();
    if (!queueCarLicense.isEmpty())
    {
        frame60Bs = queueCarLicense.dequeue();
    }
    mutex.unlock();
    return frame60Bs;
}

bool ReceiveLicensePlate::HasData()
{
    bool hasdata;
    mutex.lock();
    if(!queueCarLicense.isEmpty())
    {
        hasdata = true;
    }else{
        hasdata = false;
    }
    mutex.unlock();
    return hasdata;
}
