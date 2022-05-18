#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/IPowerOnTimerMsgDataManager.hpp"
#include "protocol.hpp"

#include <memory>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe if ISubject (TelloTelemetry) has received new PowerOnTimer message.
     * If so, parse it and set the new parsed data in IPowerOnTimerMsgDataManager.
     */
    class TelloPowerOnTimerMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new Tello PowerOnTimerMsg strength Msg Observer object.
         * 
         * @param telemetry ISubject that this object attach to.
         * @param power_on_timer_msg_data_mgr IPowerOnTimerMsgDataManager Interface that TelloDataManager supplies, for inserting new Wifi strencgh message.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloPowerOnTimerMsgObserver(ISubject &telemetry, IPowerOnTimerMsgDataManager &power_on_timer_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloPowerOnTimerMsgObserver();

    private:
        ISubject &m_telemetry;
        IPowerOnTimerMsgDataManager &m_power_on_timer_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol
