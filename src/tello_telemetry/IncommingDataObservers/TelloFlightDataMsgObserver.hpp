#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/IFlightDataMsgDataManager.hpp"
#include "protocol.hpp"
#include "flight_data/FlightData.hpp"
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe if ISubject (TelloTelemetry) has received new LogDataMsg.
     * If so, parse it and set the new parsed data in ILogDataMsgDataManager.
     */
    class TelloFlightDataMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new Tello Log Data Msg Observer object.
         * 
         * @param telemetry ISubject that this object attach to.
         * @param log_data_msg_data_mgr ILogDataMsgDataManager Interface that TelloDataManager supplies, for inserting new LOG data.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloFlightDataMsgObserver(ISubject &telemetry, IFlightDataMsgDataManager &flight_data_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloFlightDataMsgObserver();

    private:
        IFlightDataMsgDataManager &m_flight_data_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<tello_protocol::FlightData> m_flight_data_processor;
    };
} // namespace tello_protocol
