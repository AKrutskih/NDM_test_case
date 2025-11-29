#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

#include <netinet/in.h>

class TcpSocket
{
public:
    TcpSocket();
    ~TcpSocket();

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& other) = delete;
    TcpSocket& operator=(TcpSocket&& other) = delete;

    int init(int port);
    int acceptClient(sockaddr_in& clientAddr);
    int sendToClient(int clientFd, const char* buffer, size_t size);
    int receiveFromClient(int clientFd, char* buffer, size_t size);

    int fd() const { return _socketFd; }

private:
    int _socketFd;
};

#endif