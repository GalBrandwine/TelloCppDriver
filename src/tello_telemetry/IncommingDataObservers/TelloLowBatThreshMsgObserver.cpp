#include "TelloLowBatThreshMsgObserver.hpp"

namespace tello_protocol
{
    void TelloLowBatThreshMsgObserver::Update(const std::vector<unsigned char> &message_from_subject)
    {

        unsigned short cmd;
        std::memcpy(&cmd, &message_from_subject[5], sizeof(unsigned short));

        if (cmd == tello_protocol::LOW_BAT_THRESHOLD_MSG)
        {
            char limit;
            std::memcpy(&limit, &message_from_subject[10], sizeof(char));
            m_logger->info("LOW_BAT_THRESHOLD_MSG received: {}", std::to_string(limit));
            m_low_batt_thresh_msg_data_mgr.SetLowBatThreshLimit(limit);
        }
    }

    TelloLowBatThreshMsgObserver::TelloLowBatThreshMsgObserver(
        ISubject &telemetry,
        ILowBatThreshMsgDataManager &low_batt_thresh_msg_data_mgr,
        std::shared_ptr<spdlog::logger> logger,
        spdlog::level::level_enum lvl)
        : m_low_batt_thresh_msg_data_mgr(low_batt_thresh_msg_data_mgr),
          m_logger(logger)
    {
        m_logger->set_level(lvl);
        m_logger->info(m_logger->name() + " Initiated.");

        telemetry.Attach(this);
    }

    TelloLowBatThreshMsgObserver::~TelloLowBatThreshMsgObserver()
    {
        m_logger->info(m_logger->name() + " Destructed.");
    }
} // namespace tello_protocol
