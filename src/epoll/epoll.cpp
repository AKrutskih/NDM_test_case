#include <cstdio>
#include <unistd.h>

#include "epoll.hpp"

Epoll::Epoll()
{
    printf("Epoll constructed\n");
}

Epoll::~Epoll()
{
    if (_epollFd >= 0)
    {
        ::close(_epollFd);
    }
}

int Epoll::init()
{
    _epollFd = ::epoll_create1(0);
    if (_epollFd < 0)
    {
        ::printf("Failed to create epoll instance\n");
    }
}

int Epoll::addFd(int fd, uint32_t events)
{
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    return ::epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event);
}

int Epoll::updateFd(int fd, uint32_t events)
{
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    return ::epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event);
}

int Epoll::deleteFd(int fd)
{
    return ::epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, nullptr);
}

int Epoll::wait(epoll_event* events, int maxEvents, int timeout)
{
    return ::epoll_wait(_epollFd, events, maxEvents, timeout);
}

int Epoll::fd()
{
    return _epollFd;
}