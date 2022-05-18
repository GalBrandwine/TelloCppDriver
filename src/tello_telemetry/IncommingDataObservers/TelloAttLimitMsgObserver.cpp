#include "TelloAttLimitMsgObserver.hpp"

namespace tello_protocol
{
    void TelloAttLimitMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::ATT_LIMIT_MSG)
        {
            char limit;
            std::memcpy(&limit, &message_from_subject[12], sizeof(char));
            m_logger->info("ATT_LIMIT_MSG received: {}", std::to_string(limit));
            m_att_limit_msg_data_mgr.SetAttLimit(limit);
            //cc 60 00 27 b0 58 10 00 00 00 30 35
        }
    }

    TelloAttLimitMsgObserver::TelloAttLimitMsgObserver(ISubject &telemetry, IAttLimitMsgDataManager &att_limit_msg_data_mgr, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_telemetry(telemetry),
          m_att_limit_msg_data_mgr(att_limit_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        this->m_telemetry.Attach(this);
    }

    TelloAttLimitMsgObserver::~TelloAttLimitMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol
