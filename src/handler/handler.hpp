#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <string>
#include <unordered_set>

class Handler
{
public:
    Handler();

    int handleMsg(std::string& message);
    int addConnection(const std::string& connId);
    int deleteConnection();

private:
    int curConnection {0};
    std::unordered_set<std::string> allConnectionSet {};
};

#endif