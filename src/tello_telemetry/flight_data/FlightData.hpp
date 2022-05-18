#pragma once
#include <iostream>
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "utils/telemetry_data/TelemetryData.hpp"
#include "IFlightDataGetter.hpp"
namespace tello_protocol
{

    class FlightData : public IFlightDataGetter
    {
    public:
        /**
         * @brief Fill the Flight Data struct
         * 
         * @param[out] flight_data_out 
         */
        void GetFlightData(tello_protocol::FlightDataStruct &flight_data_out) override;

        bool Update(const std::vector<unsigned char> &data);

        FlightData(std::shared_ptr<spdlog::logger>);
        ~FlightData();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
       
        // double m_wifi_disturb = -1;
        // short m_wifi_strength = -1;
    
        FlightDataStruct m_flight_data;
    };
} // namespace tello_protocol