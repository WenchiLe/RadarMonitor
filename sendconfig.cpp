#include "sendconfig.h"

SendConfig::SendConfig()
{

}

void SendConfig::run()
{
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            emit SendConfigMsg(false);
            return;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        //"192.168.1.100"
        iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            emit SendConfigMsg(false);
            return;
        }


        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                printf("in for: socket failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                emit SendConfigMsg(false);
                return;
            }

            // Connect to server.
            iResult = ::connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            printf("out of for:socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            emit SendConfigMsg(false);
            return;
        }

        // Send an initial buffer
        for(auto scaleSetInfo:ScaleSetInfoS)
        {
            //printf("Bytes sent: %s\n", sendbuf);
            //iResult = send(ConnectSocket, sendbuf, DEFAULT_BUFLEN, 0);
            iResult = send(ConnectSocket, (char*)&scaleSetInfo, sizeof(scaleSetInfo), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                emit SendConfigMsg(false);
                return;
            }
            //printf("Bytes Sent: %ld\n", iResult);
            //msleep(3000);

        }
        // shutdown the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_RECEIVE);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            emit SendConfigMsg(false);
            return;
        }

        closesocket(ConnectSocket);

        // cleanup
        freeaddrinfo(result);

        WSACleanup();

        emit SendConfigMsg(true);
}

void SendConfig::GetConfig(QVector<SendConfig::ScaleSetInfo> ScaleSetInfoS)
{
    this->ScaleSetInfoS = ScaleSetInfoS;
}
