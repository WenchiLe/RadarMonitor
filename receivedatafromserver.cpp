#include <iostream>
#include <QThread>
#include "receivedatafromserver.h"

ReceiveDataFromServer::ReceiveDataFromServer()
{
    client = new QTcpSocket(this);
    in.setDevice(client);
    in.setVersion(QDataStream::Qt_5_10);
    connect(client, &QIODevice::readyRead, this, &ReceiveDataFromServer::readData);
    connect(client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ReceiveDataFromServer::displayError);
    readHeadOrBody = "head";
}



void ReceiveDataFromServer::CarLicenseHandler(const char *buffer)
{
    const CarLicense *carLicense = reinterpret_cast<const CarLicense *>(buffer);
    //std::cout<<"license: "<<carLicense->license<<std::endl;
    // todo
    mutexCarLicense.lock();
    queueCarLicense.enqueue(*carLicense);
    mutexCarLicense.unlock();
}

void ReceiveDataFromServer::Frame60BsHandler(const char *buffer)
{
    const FrameStructData *frame60Bs = reinterpret_cast<const FrameStructData *>(buffer);
    //std::cout<<"frame: "<<frame60Bs->length<<std::endl;
    // todo
    mutexFrame60Bs.lock();
    queueFrame60Bs.enqueue(*frame60Bs);
    mutexFrame60Bs.unlock();
}

void ReceiveDataFromServer::readData()
{
    if ("head" == readHeadOrBody)
    {
    if (client->bytesAvailable() < sizeof(MagicHeader))
    {
        return;
    }
    in.readRawData(reinterpret_cast<char *>(&magicHeader), sizeof(MagicHeader));
    //std::cout<<"head number: "<<magicHeader.number<<std::endl;
    readHeadOrBody = "body";
    }
    if (client->bytesAvailable() < magicHeader.packetLength)
    {
    return;
    }

    if (magicHeader.packetLength > 0)
    {
    char buffer[magicHeader.packetLength];
    in.readRawData(buffer, magicHeader.packetLength);

    switch (magicHeader.number)
    {
    case 0x20:
        CarLicenseHandler(buffer);
        break;
    case 0x30:
        Frame60BsHandler(buffer);
        break;
    default:
        // todo: error
        break;

    }
    readHeadOrBody = "head";
    }

    readData();
}

void ReceiveDataFromServer::run()
{
    std::cout << "thread run !" << std::endl;
    MagicHeader head;
    head.number = 0x40;
    head.packetLength = sizeof(LiveView);
    LiveView liveView;
    liveView.clientId = 19950310;
    client->write(reinterpret_cast<const char *>(&head), sizeof(head));
    client->write(reinterpret_cast<const char *>(&liveView), sizeof(liveView));
}

void ReceiveDataFromServer::StartReceiveData()
{
    //client->connectToHost(QHostAddress("192.168.233.1"), 1995);
    //client->connectToHost(QHostAddress("118.25.49.212"), 1995);
    QHostInfo::lookupHost(DEFAULT_SERVER,
              this, SLOT(lookedUp(QHostInfo)));
    //this->start();
}

void ReceiveDataFromServer::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
    break;
    case QAbstractSocket::HostNotFoundError:
    std::cout << "The host was not found. Please check the host name and port settings." << std::endl;
    break;
    case QAbstractSocket::ConnectionRefusedError:
    std::cout << "The connection was refused by the peer. "
          "Make sure the fortune server is running, "
          "and check that the host name and port "
          "settings are correct." << std::endl;
    break;
    default:
    std::cout << "The following error occurred: %1." << std::endl;
    }
}

FrameStructData ReceiveDataFromServer::GetQueueFrame60Bs()
{
    FrameStructData frame60Bs;
    frame60Bs.length = 0;
    mutexFrame60Bs.lock();
    if (!queueFrame60Bs.isEmpty())
    {
    frame60Bs = queueFrame60Bs.dequeue();
    }
    mutexFrame60Bs.unlock();
    return frame60Bs;
}

bool ReceiveDataFromServer::HasFrame60Bs()
{
    bool hasdata;
    mutexFrame60Bs.lock();
    if (!queueFrame60Bs.isEmpty())
    {
    hasdata = true;
    }
    else
    {
    hasdata = false;
    }
    mutexFrame60Bs.unlock();
    return hasdata;
}

ReceiveDataFromServer::CarLicense ReceiveDataFromServer::GetQueueCarLicense()
{
    CarLicense carLicense;
    mutexCarLicense.lock();
    if (!queueCarLicense.isEmpty())
    {
    carLicense = queueCarLicense.dequeue();
    }
    mutexCarLicense.unlock();
    return carLicense;
}

bool ReceiveDataFromServer::HasCarLicense()
{
    bool hasdata;
    mutexCarLicense.lock();
    if (!queueCarLicense.isEmpty())
    {
    hasdata = true;
    }
    else
    {
    hasdata = false;
    }
    mutexCarLicense.unlock();
    return hasdata;
}

void ReceiveDataFromServer::lookedUp(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError)
    {
    qDebug() << "Lookup failed:" << host.errorString();
    return;
    }

    foreach (const QHostAddress &address, host.addresses())
    {
    // 输出IPV4、IPv6地址
    if (address.protocol() == QAbstractSocket::IPv4Protocol)
    {
        qDebug() << "Found IPv4 address:" << address.toString();
        client->connectToHost(QHostAddress(address.toString()), DEFAULT_PORT);
        this->start();
        break;
    }
    else if (address.protocol() == QAbstractSocket::IPv6Protocol)
    {
        qDebug() << "Found IPv6 address:" << address.toString();
        client->connectToHost(QHostAddress(address.toString()), DEFAULT_PORT);
        this->start();
        break;
    }
    else
    {
        qDebug() << "Found other address:" << address.toString();
        client->connectToHost(QHostAddress(address.toString()), DEFAULT_PORT);
        this->start();
        break;
    }

    }
}
