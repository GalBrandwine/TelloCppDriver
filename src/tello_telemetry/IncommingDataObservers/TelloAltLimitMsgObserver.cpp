#include "TelloAltLimitMsgObserver.hpp"

namespace tello_protocol
{
    void TelloAltLimitMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::ALT_LIMIT_MSG)
        {
            char limit;
            std::memcpy(&limit, &message_from_subject[10], sizeof(char));
            m_logger->info("ALT_LIMIT_MSG received: {}", std::to_string(limit));
            m_alt_limit_msg_data_mgr.SetAltLimit(limit);
        }
    }

    TelloAltLimitMsgObserver::TelloAltLimitMsgObserver(ISubject &telemetry, IAltLimitMsgDataManager &alt_limit_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_alt_limit_msg_data_mgr(alt_limit_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        telemetry.Attach(this);
    }

    TelloAltLimitMsgObserver::~TelloAltLimitMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol
