#pragma once
#include "vector"
#include "asio.hpp"
using asio::ip::udp;

class IVisReceiver
{
public:
    virtual int Receive(std::vector<unsigned char> &) = 0;
    virtual ~IVisReceiver(){};
};