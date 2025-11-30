#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

class UdpSocket
{
public:
    UdpSocket();
    ~UdpSocket();

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;
    UdpSocket(UdpSocket&& other) = delete;
    UdpSocket& operator=(UdpSocket&& other) = delete;

    int init(int port);
    int receive(char* buffer, size_t size, sockaddr_in& clientAddr);
    int send(const char* buffer, size_t size, const sockaddr_in& clientAddr);
    int fd();

private:
    int _socketFd {-1};
};

#endif