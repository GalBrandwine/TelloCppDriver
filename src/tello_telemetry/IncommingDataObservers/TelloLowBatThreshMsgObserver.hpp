#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/ILowBatThreshMsgDataManager.hpp"

#include "protocol.hpp"
#include <memory>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe if ISubject (TelloTelemetry) has received new LogDataMsg.
     * If so, parse it and set the new parsed data in ILowBatThreshMsgDataManager.
     */
    class TelloLowBatThreshMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new LOW_BAT_THRESHOLD_MSG Observer object.
         * 
         * @param telemetry ISubject that this object attach to.
         * @param low_bat_thresh_msg_data_mgr ILowBatThreshMsgDataManager Interface that TelloDataManager supplies, for storing drone's low battery thresh message.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloLowBatThreshMsgObserver(ISubject &telemetry, ILowBatThreshMsgDataManager &low_bat_thresh_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloLowBatThreshMsgObserver();

    private:
        ILowBatThreshMsgDataManager &m_low_batt_thresh_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol
