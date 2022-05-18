#pragma once
#include <vector>
#include "utils/telemetry_data/TelemetryData.hpp"
class IPowerOnTimerMsgDataManager
{
public:
    virtual void SetPowerOnTimer(const tello_protocol::PowerOnTimerInfo &power_on_timer) = 0;
    virtual ~IPowerOnTimerMsgDataManager(){};
};