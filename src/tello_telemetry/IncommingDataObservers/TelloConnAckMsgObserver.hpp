#pragma once
#include "utils/tello_observer/IObserver.hpp"
#include "utils/tello_observer/ISubject.hpp"
#include "utils/data_manager/IConnAckMsgDataManager.hpp"

#include <memory>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace tello_protocol
{
    /**
     * @brief Observe if ISubject (TelloTelemetry) has received new LogDataMsg.
     * If so, parse it and set the new parsed data in IConnAckMsgDataManager.
     */
    class TelloConnAckMsgObserver : public IObserver
    {
    public:
        void Update(const std::vector<unsigned char> &message_from_subject) override;

        /**
         * @brief Construct a new Tello Connection-request-acknowledeg Msg Observer object.
         * 
         * @param telemetry ISubject that this object attach to.
         * @param conn_ack_msg_data_mgr IConnAckMsgDataManager Interface that TelloDataManager supplies, for inserting new Connection-request-acknowledeg.
         * @param logger spdlog::logger
         * @param lvl spdlog::level
         */
        TelloConnAckMsgObserver(ISubject &telemetry, IConnAckMsgDataManager &conn_ack_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl = spdlog::level::info);
        ~TelloConnAckMsgObserver();

    private:
        ISubject &m_telemetry;
        IConnAckMsgDataManager &m_conn_ack_msg_data_mgr;
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace tello_protocol
