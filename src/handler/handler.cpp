#include <cstdio>
#include <ctime>

#include "common.hpp"
#include "handler.hpp"

extern Status status;

Handler::Handler()
{
    ::printf("Handler constructed\n");
}

int Handler::handleMsg(std::string& message)
{
    if (message.empty())
    {
        return -1;
    }

    if (message[0] == '/')
    {
        if (message == "/time")
        {
            std::time_t now = std::time(nullptr);
            char buf[100];

            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

            message = buf;
        }
        else if (message == "/stats")
        {
            message = "Total connections: " + std::to_string(allConnectionSet.size()) +
                      ", Current connections: " + std::to_string(curConnection);
        }
        else if (message == "/shutdown")
        {
            status = Status::SHUTDOWN;
            message = "Server is shutsown";
        }
        else
        {
            ::printf("Unknown command: %s\n", message.c_str());
            return -1;
        }
    }

    return 0;
}

int Handler::addConnection(const std::string& connId)
{
    allConnectionSet.insert(connId);
    ++curConnection;

    return 0;
}

int Handler::deleteConnection()
{
    if (curConnection > 0)
    {
        --curConnection;
    }

    return 0;
}