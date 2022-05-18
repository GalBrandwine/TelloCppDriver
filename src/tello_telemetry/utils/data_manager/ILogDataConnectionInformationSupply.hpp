#pragma once
#include "utils/telemetry_data/TelemetryData.hpp"

class ILogDataConnectionInformationSupply
{
public:
        virtual tello_protocol::ConnectionInformation &GetConnectionInformation() = 0;
        virtual ~ILogDataConnectionInformationSupply(){};
};
