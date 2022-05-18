#pragma once
#include <vector>
#include "protocol.hpp"
class IWifiMsgDataManager
{
public:
    virtual void SetWifiMsg(const unsigned char &wifi_strength) = 0;
    virtual ~IWifiMsgDataManager(){};
};
