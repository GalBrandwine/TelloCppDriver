#pragma once
#include <string>
#include "asio.hpp"
using asio::ip::udp;
class ISender
{
public:
    virtual void Send(const std::string &) = 0;
    virtual ~ISender(){};
};
