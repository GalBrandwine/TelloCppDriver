#pragma once
#include "vector"
#include "asio.hpp"
using asio::ip::udp;
class IReceiver
{
public:
    virtual int Receive(std::vector<unsigned char> &) = 0;
    virtual ~IReceiver(){};
};