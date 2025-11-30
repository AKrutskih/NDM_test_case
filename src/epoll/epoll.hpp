#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>

class Epoll
{
public:
    Epoll();
    ~Epoll();

    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;
    Epoll(Epoll&& other) = delete;
    Epoll& operator=(Epoll&& other) = delete;

    int init();
    int addFd(int fd, uint32_t events);
    int updateFd(int fd, uint32_t events);
    int deleteFd(int fd);
    int wait(epoll_event* events, int maxEvents, int timeout);
    int fd();

private:
    int _epollFd {-1};
};

#endif