#include "TelloConnAckMsgObserver.hpp"

namespace tello_protocol
{
    void TelloConnAckMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        std::string is_acked(message_from_subject.begin(), message_from_subject.begin() + 8);

        if (is_acked == "conn_ack")
        {
            m_logger->info("Connection request acknowledged!");
            m_conn_ack_msg_data_mgr.SetConnReqAck();
        }

    }

    TelloConnAckMsgObserver::TelloConnAckMsgObserver(ISubject &telemetry, IConnAckMsgDataManager &conn_ack_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_telemetry(telemetry),
          m_conn_ack_msg_data_mgr(conn_ack_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        this->m_telemetry.Attach(this);
    }

    TelloConnAckMsgObserver::~TelloConnAckMsgObserver()
    {
    }
} // namespace tello_protocol
