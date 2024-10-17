#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/IAltLimitMsgDataManager.hpp"

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
     * If so, parse it and set the new parsed data in IAltLimitMsgDataManager.
     */
    class TelloAltLimitMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new AltLimitMsg Observer object.
         * 
         * @param telemetry ISubject that this object attach to.
         * @param alt_limit_msg_data_mgr IAltLimitMsgDataManager Interface that TelloDataManager supplies, for inserting new AltLimit message.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloAltLimitMsgObserver(ISubject &telemetry, IAltLimitMsgDataManager &alt_limit_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloAltLimitMsgObserver();

    private:
        IAltLimitMsgDataManager &m_alt_limit_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol
