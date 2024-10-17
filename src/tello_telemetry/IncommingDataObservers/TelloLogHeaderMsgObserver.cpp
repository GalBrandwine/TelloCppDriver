#include "TelloLogHeaderMsgObserver.hpp"

namespace tello_protocol
{
    void TelloLogHeaderMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {
        tello_protocol::Packet received(message_from_subject);

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::LOG_HEADER_MSG)
        {
            m_logger->debug("LOG_HEADER_MSG received");

            unsigned short log_id;
            std::memcpy(&log_id, &message_from_subject[9], sizeof(unsigned short));

            m_log_header_msg_data_mgr.SetLogID(log_id);

            auto build_str = received.GetBuffer().substr(28, 26);
            std::vector<unsigned char> build_date(build_str.begin(), build_str.end());
            m_log_header_msg_data_mgr.SetBuildDate(build_date);

            auto log = received.GetBuffer().substr(received.GetBuffer().find("DJI"), 15);
            std::vector<unsigned char> log_version(log.begin(), log.end());
            m_log_header_msg_data_mgr.SetDJILogVersion(log_version);
        }
    }

    TelloLogHeaderMsgObserver::TelloLogHeaderMsgObserver(ISubject &telemetry, ILogHeaderMsgDataManager &log_header_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_log_header_msg_data_mgr(log_header_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        telemetry.Attach(this);
    }

    TelloLogHeaderMsgObserver::~TelloLogHeaderMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol
