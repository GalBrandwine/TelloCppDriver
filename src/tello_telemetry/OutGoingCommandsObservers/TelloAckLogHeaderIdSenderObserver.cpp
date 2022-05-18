#include "TelloAckLogHeaderIdSenderObserver.hpp"

namespace tello_protocol
{
    void TelloAckLogHeaderIdSenderObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {
        unsigned short id;
        std::memcpy(&id, &message_from_subject, sizeof(unsigned short));
        m_commander.SendAckLog(id);
    }

    TelloAckLogHeaderIdSenderObserver::TelloAckLogHeaderIdSenderObserver(ISendAckLog &commander, IDataMgrSubject &data_mgr_subject, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum lvl)
        : m_commander(commander),
          m_data_mgr_subject(data_mgr_subject),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        this->m_data_mgr_subject.Attach(OBSERVERS::ACK_LOG_HEADER, this);
    }

    TelloAckLogHeaderIdSenderObserver::~TelloAckLogHeaderIdSenderObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol
