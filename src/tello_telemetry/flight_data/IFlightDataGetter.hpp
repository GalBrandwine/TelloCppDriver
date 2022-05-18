#pragma once
#include <memory>
#include "utils/telemetry_data/TelemetryData.hpp"
class IFlightDataGetter
{
public:
    virtual void GetFlightData(tello_protocol::FlightDataStruct& flight_data_out) = 0;
    virtual ~IFlightDataGetter(){};
};
