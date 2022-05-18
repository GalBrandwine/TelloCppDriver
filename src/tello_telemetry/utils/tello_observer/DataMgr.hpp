#pragma once
#include <string>
enum OBSERVERS
{
    ACK_LOG_HEADER,
    DJI_LOG_VERSION,       //< Notify all attached that Log build data and version, has been received.
    POSITION_VELOCITY_LOG, //< Notify all attached that position and velocity, has been received.
    IMU_ATTITUDE_LOG,
    FLIGHT_DATA_MSG
};

static const std::string observer_name(OBSERVERS observer_type)
{
    switch (observer_type)
    {
    case OBSERVERS::ACK_LOG_HEADER:
        return "ACK_LOG_HEADER";
    case OBSERVERS::DJI_LOG_VERSION:
        return "DJI_LOG_VERSION";
    case OBSERVERS::POSITION_VELOCITY_LOG:
        return "POSITION_VELOCITY_LOG";
    case OBSERVERS::IMU_ATTITUDE_LOG:
        return "IMU_ATTITUDE_LOG";
    case OBSERVERS::FLIGHT_DATA_MSG:
        return "FLIGHT_DATA_MSG";        
        
    default:
        return "NOT IMPLEMENTED";
    }
}