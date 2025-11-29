#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "tcp_socket.hpp"

static constexpr int LISTEN_QUEUE = 100;

TcpSocket::TcpSocket()
{
    _socketFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd < 0)
    {
        ::printf("Failed to create TCP socket");
    }

    printf("TCP socket constructed\n");
}

TcpSocket::~TcpSocket()
{
    if (_socketFd >= 0)
    {
        ::close(_socketFd);
    }

    printf("TCP socket destructed\n");
}

int TcpSocket::init(int port)
{
    sockaddr_in tcpAddr{};
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = INADDR_ANY;
    tcpAddr.sin_port = ::htons(port);

    int opt = 1;
    ::setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (::bind(_socketFd, (sockaddr*)&tcpAddr, sizeof(tcpAddr)) < 0)
    {
        ::printf("Failed to bind TCP socket");
        return -1;
    }

    if (::listen(_socketFd, LISTEN_QUEUE) < 0)
    {
        ::printf("Failed to listen on TCP socket");
        return -1;
    }

    int flags = ::fcntl(_socketFd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return ::fcntl(_socketFd, F_SETFL, flags | O_NONBLOCK);
}

int TcpSocket::acceptClient(sockaddr_in& clientAddr)
{
    socklen_t clientLen = sizeof(clientAddr);
    return ::accept(_socketFd, (sockaddr*)&clientAddr, &clientLen);
}

int TcpSocket::sendToClient(int clientFd, const char* buffer, size_t size)
{
    return ::send(clientFd, buffer, size, 0);
}

int TcpSocket::receiveFromClient(int clientFd, char* buffer, size_t size)
{
    return ::recv(clientFd, buffer, size, 0);
}