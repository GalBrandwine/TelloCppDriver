#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/IWifiMsgDataManager.hpp"

#include <memory>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe if ISubject (TelloTelemetry) has received new LogDataMsg.
     * If so, parse it and set the new parsed data in ILogDataMsgDataManager.
     */
    class TelloWifiMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new Tello Wifi strength Msg Observer object.
         *
         * @param telemetry ISubject that this object attach to.
         * @param wifi_msg_data_mgr IWifiMsgDataManager Interface that TelloDataManager supplies, for inserting new Wifi strength message.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloWifiMsgObserver(ISubject &telemetry, IWifiMsgDataManager &wifi_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloWifiMsgObserver();

    private:
        IWifiMsgDataManager &m_wifi_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol
