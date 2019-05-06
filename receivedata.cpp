#include "receivedata.h"

ReceiveData::ReceiveData()
{

}

void ReceiveData::StartReceiveData()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("radar: WSAStartup failed with error: %d\n", iResult);
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
        printf("radar: getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("radar: socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("radar: bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("radar: listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    ListenFlag = 1;

    this->start();
}

void ReceiveData::run()
{
    while (ListenFlag == 1)
    {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("radar: accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        //Receive until the peer shuts down the connection
        do
        {
            Frame60Bs frame60Bs;
            iResult = recv(ClientSocket, (char*)&frame60Bs, sizeof(frame60Bs), 0);
            if (iResult > 0) {
                //printf("radar: Bytes received: %d\n", iResult);
                if (iResult == sizeof(Frame60Bs))
                {
                    //std::cout << "data.length:" << frame60Bs.length << std::endl;
                    //for (int j = 0;j < frame60Bs.length;j++)
                    //{
                    //    cout << "ObjID:" << frame60Bs.frame[j][0] << endl;
                    //    cout << "ObjDistLong:" << frame60Bs.frame[j][1] << endl;
                    //    cout << "ObjDistLat:" << frame60Bs.frame[j][2] << endl;
                    //    cout << "ObjVelocityLong:" << frame60Bs.frame[j][3] << endl;
                    //    cout << "ObjvelocityLat:" << frame60Bs.frame[j][4] << endl;
                    //}
                    mutex.lock();
                    queueFrame60Bs.enqueue(frame60Bs);
                    mutex.unlock();
                }
                else {
                    std::cout << "radar: package lost" << std::endl;
                }
            }
            else if (iResult == 0)
                printf("radar: no data...\n");
            else {
                printf("radar: recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
            }
        } while (iResult > 0);

        iResult = shutdown(ClientSocket, SD_RECEIVE);
        if (iResult == SOCKET_ERROR) {
            printf("radar: shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
        }
        closesocket(ClientSocket);
    }
}

void ReceiveData::StopReceiveData()
{
    ListenFlag = 0;

    // cleanup
    closesocket(ListenSocket);

    WSACleanup();

    if(!this->wait(5000))
    {
        qWarning("radar:ReceiveData : Thread deadlock detected");
        this->terminate();
        this->wait();
    }
}


ReceiveData::Frame60Bs ReceiveData::GetQueue()
{
    Frame60Bs frame60Bs;
    frame60Bs.length = 0;
    mutex.lock();
    if (!queueFrame60Bs.isEmpty())
    {
        frame60Bs = queueFrame60Bs.dequeue();
    }
    mutex.unlock();
    return frame60Bs;
}

bool ReceiveData::HasData()
{
    bool hasdata;
    mutex.lock();
    if(!queueFrame60Bs.isEmpty())
    {
        hasdata = true;
    }else{
        hasdata = false;
    }
    mutex.unlock();
    return hasdata;
}

