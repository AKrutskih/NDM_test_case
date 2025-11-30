#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string>

#include "src/common.hpp"
#include "src/udp/udp_socket.hpp"
#include "src/tcp/tcp_socket.hpp"
#include "src/epoll/epoll.hpp"
#include "src/handler/handler.hpp"

/* @todo to config */
static constexpr int PORT = 9999;
static constexpr int BUFFER_SIZE = 1024;
static constexpr int MAX_EVENTS = 1024;

Status status = Status::STOP;

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

    Handler handler {};

    retVal = epoll.addFd(udpSocket.fd(), EPOLLIN | EPOLLET);
    CHECK_RETVAL(retVal);
    retVal = epoll.addFd(tcpSocket.fd(), EPOLLIN | EPOLLET);
    CHECK_RETVAL(retVal);

    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    status = Status::RUN;
    ::printf("Server started on port %d\n", PORT);

    while (status == Status::RUN)
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
                int recSize = udpSocket.receive(buffer, BUFFER_SIZE, clientAddr);
                if (recSize > 0)
                {
                    std::string ipPort = std::string(inet_ntoa(clientAddr.sin_addr)) + ":" + std::to_string(ntohs(clientAddr.sin_port));
                    handler.addConnection(ipPort);

                    ::printf("Received \"%.*s\" from Udp %s\n", recSize, buffer, ipPort.c_str());

                    std::string message(buffer, recSize);

                    if (handler.handleMsg(message) < 0)
                    {
                        ::printf("Handler failed\n");
                        continue;
                    }

                    retVal = udpSocket.send(message.c_str(), message.size(), clientAddr);
                    CHECK_RETVAL(retVal);

                    handler.deleteConnection();
                }
            }
            /* tcp new */
            else if (fd == tcpSocket.fd())
            {
                sockaddr_in clientAddr{};
                int clientFd = tcpSocket.acceptClient(clientAddr);
                if (clientFd >= 0)
                {
                    std::string ipPort = std::string(inet_ntoa(clientAddr.sin_addr)) + ":" + std::to_string(ntohs(clientAddr.sin_port));
                    handler.addConnection(ipPort);

                    ::printf("New TCP client connected from %s\n", ipPort.c_str());

                    TcpSocket::nonBlockingFd(clientFd);

                    epoll.addFd(clientFd, EPOLLIN | EPOLLET);
                }
            }
            /* tcp data */
            else
            {
                int recSize = tcpSocket.receive(fd, buffer, BUFFER_SIZE);
                if (recSize > 0)
                {
                    ::printf("Received \"%.*s\" from TCP client\n", recSize, buffer);

                    std::string message(buffer, recSize);

                    if (handler.handleMsg(message) < 0)
                    {
                        ::printf("Handler failed\n");
                        continue;
                    }

                    tcpSocket.send(fd, message.c_str(), message.size());
                }
                else
                {
                    ::printf("TCP client disconnected\n");
                    handler.deleteConnection();
                    epoll.deleteFd(fd);
                    ::close(fd);
                }
            }
        }
    }

    ::printf("Server closed\n");

    return 0;
}