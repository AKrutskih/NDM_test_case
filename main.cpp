#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <string>

#include "src/common.hpp"
#include "src/udp/udp_socket.hpp"
#include "src/tcp/tcp_socket.hpp"
#include "src/epoll/epoll.hpp"

/* @todo to config */
static constexpr int PORT = 9999;
static constexpr int BUFFER_SIZE = 1024;
static constexpr int MAX_EVENTS = 1024;

int main()
{
    int retVal {0};

    UdpSocket udpSocket {};
    retVal = udpSocket.init(PORT);
    CHECK_RETVAL(retVal);

    TcpSocket tcpSocket {};
    retVal = tcpSocket.init(PORT);
    CHECK_RETVAL(retVal);

    Epoll epoll {};
    retVal = epoll.init();
    CHECK_RETVAL(retVal);

    retVal = epoll.addFd(udpSocket.fd(), EPOLLIN | EPOLLET);
    CHECK_RETVAL(retVal);
    retVal = epoll.addFd(tcpSocket.fd(), EPOLLIN | EPOLLET);
    CHECK_RETVAL(retVal);

    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    ::printf("Server started on port %d\n", PORT);

    while (true)
    {
        int n = epoll.wait(events, MAX_EVENTS, -1);
        CHECK_RETVAL(n);

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;

            /* udp */
            if (fd == udpSocket.fd())
            {
                sockaddr_in clientAddr{};
                int receivedData = udpSocket.receive(buffer, BUFFER_SIZE, clientAddr);
                if (receivedData > 0)
                {
                    ::printf("Received \"%.*s\" from Udp port %d\n", receivedData, buffer, ntohs(clientAddr.sin_port));

                    retVal = udpSocket.send(buffer, receivedData, clientAddr); /* @todo */
                    CHECK_RETVAL(retVal);
                }
            }
            /* tcp new */
            else if (fd == tcpSocket.fd())
            {
                sockaddr_in clientAddr{};
                int clientFd = tcpSocket.acceptClient(clientAddr);
                if (clientFd >= 0)
                {
                    ::printf("New TCP client connected from port %d\n", ntohs(clientAddr.sin_port));

                    TcpSocket::nonBlockingFd(clientFd);

                    epoll.addFd(clientFd, EPOLLIN | EPOLLET);
                }
            }
            /* tcp data */
            else
            {
                int bytes = tcpSocket.receive(fd, buffer, BUFFER_SIZE);
                if (bytes > 0)
                {
                    ::printf("Received \"%.*s\" from TCP client\n", bytes, buffer);

                    tcpSocket.send(fd, buffer, bytes); /* @todo */
                }
                else
                {
                    ::printf("TCP client disconnected\n");
                    epoll.deleteFd(fd);
                    ::close(fd);
                }
            }
        }
    }

    ::printf("Server closed\n");

    return 0;
}