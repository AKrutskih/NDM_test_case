#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <string>

#include "udp_socket.hpp"
#include "tcp_socket.hpp"

static constexpr int PORT = 9999;
static constexpr int BUFFER_SIZE = 1024;
static constexpr int MAX_EVENTS = 1024;

int main()
{
    UdpSocket udpSocket {};
    udpSocket.init(PORT);

    TcpSocket tcpSocket {};
    tcpSocket.init(PORT);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        printf("epoll_create1 failed\n");
        return -1;
    }

    epoll_event event{};

    event.events = EPOLLIN;
    event.data.fd = udpSocket.fd();

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udpSocket.fd(), &event) < 0)
    {
        printf("epoll_ctl failed\n");
        close(epoll_fd);
        return -1;
    }

    event.data.fd = tcpSocket.fd();
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcpSocket.fd(), &event) < 0)
    {
        printf("epoll_ctl failed\n");
        close(epoll_fd);
        return -1;
    }

    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    printf("Server running on port %d\n", PORT);

    while (true)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n < 0)
        {
            printf("epoll_wait failed\n");
            break;
        }

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;

            if (fd == udpSocket.fd())
            {
                sockaddr_in clientAddr{};
                int receivedData = udpSocket.receiveFrom(buffer, BUFFER_SIZE, clientAddr);
                if (receivedData > 0)
                {
                    printf("Received \"%.*s\" from Udp port %d\n", receivedData, buffer, ntohs(clientAddr.sin_port));

                    int sentData = udpSocket.sendTo(buffer, receivedData, clientAddr);
                    if (sentData < 0)
                    {
                        printf("Failed to send data\n");
                    }
                }
            }
            else if (fd == tcpSocket.fd())
            {
                sockaddr_in clientAddr{};
                int clientFd = tcpSocket.acceptClient(clientAddr);
                if (clientFd >= 0)
                {
                    printf("New TCP client connected from port %d\n", ntohs(clientAddr.sin_port));

                    epoll_event clientEvent{};
                    clientEvent.events = EPOLLIN;
                    clientEvent.data.fd = clientFd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientFd, &clientEvent);
                }
            }
            else
            {
                int bytes = tcpSocket.receiveFromClient(fd, buffer, BUFFER_SIZE);
                if (bytes > 0)
                {
                    
                    printf("Received \"%.*s\" from TCP client\n", bytes-1, buffer);
                    tcpSocket.sendToClient(fd, buffer, bytes);
                }
                else
                {
                    printf("TCP client disconnected\n");
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                }
            }
        }
    }

    return 0;
}