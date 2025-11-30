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
    int send(int clientFd, const char* buffer, size_t size);
    int receive(int clientFd, char* buffer, size_t size);
    int fd();

    static int nonBlockingFd(int fd);

private:
    int _socketFd {-1};
};

#endif