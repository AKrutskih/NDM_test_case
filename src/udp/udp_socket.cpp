#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "udp_socket.hpp"

UdpSocket::UdpSocket()
{
    _socketFd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (_socketFd < 0)
    {
        ::printf("Failed to create UDP socket");
    }

    printf("UDP socket constructed\n");
}

UdpSocket::~UdpSocket()
{
    if (_socketFd >= 0)
    {
        ::close(_socketFd);
    }

    printf("UDP socket destructed\n");
}

int UdpSocket::init(int port)
{
    sockaddr_in udpAddr{};
    udpAddr.sin_family = AF_INET;
    udpAddr.sin_addr.s_addr = INADDR_ANY;
    udpAddr.sin_port = ::htons(port);

    if (::bind(_socketFd, (sockaddr*)&udpAddr, sizeof(udpAddr)) < 0)
    {
        ::printf("Failed to bind UDP socket");
        return -1;
    }
    
    int flags = ::fcntl(_socketFd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return ::fcntl(_socketFd, F_SETFL, flags | O_NONBLOCK);
}

int UdpSocket::receiveFrom(char* buffer, size_t size, sockaddr_in& clientAddr)
{
    socklen_t client_len = sizeof(clientAddr);
    return ::recvfrom(_socketFd, buffer, size, 0, (sockaddr*)&clientAddr, &client_len);
}

int UdpSocket::sendTo(const char* buffer, size_t size, const sockaddr_in& clientAddr)
{
    socklen_t client_len = sizeof(clientAddr);
    return ::sendto(_socketFd, buffer, size, 0, (sockaddr*)&clientAddr, client_len);
}

int UdpSocket::fd()
{
    return _socketFd;
}