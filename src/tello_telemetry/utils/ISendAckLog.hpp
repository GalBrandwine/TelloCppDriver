#pragma once
#include <string>
class ISendAckLog
{
public:
    virtual void SendAckLog(const int id) = 0;
    virtual ~ISendAckLog(){};
};
